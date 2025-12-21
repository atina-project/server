#include"core/utils/request.h"

#include<cassert>
#include<sstream>

#include"curl/curl.h"

#define CURL_SAFE_SETOPT(curl, opt, data, res, errmsg)  \
    res = curl_easy_setopt(curl.get(), opt, data);      \
    if (res != CURLE_OK)                                \
    {                                                   \
        std::ostringstream oss;                         \
        oss << "Failed to set " << #opt << ": "         \
            << curl_easy_strerror(res);                 \
        errmsg = oss.str();                             \
        return 1;                                       \
    }

using namespace atina::server::core;
namespace fs = std::filesystem;

struct utils::request::_guard {

    _guard(){
        curl_global_init(CURL_GLOBAL_ALL);
        return;
    }

    ~_guard(){
        curl_global_cleanup();
        return;
    }

}; // struct utils::request::_guard

std::unique_ptr<utils::request::_guard> utils::request::_p_guard = nullptr;

void utils::request::init(){
    if (_p_guard)
    {
        return;
    } // already inited

    _p_guard = std::make_unique<_guard>();

    return;
}

int utils::request::get(
    const std::string& __cr_host,
    int __port,
    const std::string& __cr_endpoint,
    bool __https,
    int __timeout_s,
    response& __o_res,
    std::string& __o_errmsg,
    bool __crt_verify,
    const fs::path& __cr_crt_path
){
    assert(_p_guard);

    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(curl_easy_init(), curl_easy_cleanup);
    if (!curl)
    {
        __o_errmsg = "Failed to initialize curl object";
        return 1;
    }

    std::string url = _build_url(__cr_host, __port, __cr_endpoint, __https);
    std::string response_data;
    CURLcode res;

    CURL_SAFE_SETOPT(curl, CURLOPT_URL, url.c_str(), res, __o_errmsg);
    CURL_SAFE_SETOPT(curl, CURLOPT_WRITEFUNCTION, _cb_curl_write_str, res, __o_errmsg);
    CURL_SAFE_SETOPT(curl, CURLOPT_WRITEDATA, &response_data, res, __o_errmsg);

    if (__crt_verify)
    {
        CURL_SAFE_SETOPT(curl, CURLOPT_SSL_VERIFYPEER, 1L, res, __o_errmsg);
        CURL_SAFE_SETOPT(curl, CURLOPT_SSL_VERIFYHOST, 2L, res, __o_errmsg);

        if (!__cr_crt_path.empty())
        {
            CURL_SAFE_SETOPT(curl, CURLOPT_CAINFO, __cr_crt_path.c_str(), res, __o_errmsg);
        } // custom ca / crt
    } // enable crt verify
    else
    {
        CURL_SAFE_SETOPT(curl, CURLOPT_SSL_VERIFYPEER, 0L, res, __o_errmsg);
        CURL_SAFE_SETOPT(curl, CURLOPT_SSL_VERIFYHOST, 0L, res, __o_errmsg);
    } // disable crt verify

    CURL_SAFE_SETOPT(curl, CURLOPT_TIMEOUT, __timeout_s, res, __o_errmsg);
    CURL_SAFE_SETOPT(curl, CURLOPT_FOLLOWLOCATION, 1L, res, __o_errmsg);
    CURL_SAFE_SETOPT(curl, CURLOPT_USERAGENT, _get_useragent().c_str(), res, __o_errmsg);

    res = curl_easy_perform(curl.get());
    if (res != CURLE_OK)
    {
        std::ostringstream oss;
        oss << "Failed to request: " << curl_easy_strerror(res);
        __o_errmsg = oss.str();
        return 1;
    } // request failed

    long response_code = 0;
    curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &response_code);
    // should never fails

    __o_res = {response_code, response_data};
    return 0;
}

std::string utils::request::_build_url(
    const std::string& __cr_host,
    int __port,
    const std::string& __cr_endpoint,
    bool __https
){
    std::ostringstream oss;

    oss << (__https ? "https" : "http") << "://"
        << __cr_host
        << (__port != -1 ? ":" + std::to_string(__port) : "");

    if (!__cr_endpoint.empty())
    {
        oss << "/" << (__cr_endpoint[0] != '/' ? __cr_endpoint : __cr_endpoint.substr(1));
    }

    return oss.str();
}

std::string utils::request::_get_useragent(){
    curl_version_info_data* ver = curl_version_info(CURLVERSION_NOW);
    // no need to free: curl_version_info() returns a reference of a static obj
    
    std::ostringstream oss;
    oss << "libcurl/" << ((ver->version_num >> 16) & 0xFF) << "."
                      << ((ver->version_num >> 8) & 0xFF) << "."
                      << (ver->version_num & 0xFF);

    return oss.str();
}

std::size_t utils::request::_cb_curl_write_str(
    char* __p_data,
    std::size_t __size,
    std::size_t __nmemb,
    std::string* __p_userdata
){
    std::size_t realsize = __size * __nmemb;
    try {
        __p_userdata->append(__p_data, realsize);
    }
    catch (...)
    {
        return 0;
    }
    return realsize;
}
