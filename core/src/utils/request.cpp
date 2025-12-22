#include"core/utils/request.h"

#include<cassert>
#include<sstream>

#include"curl/curl.h"
#include"json/json.h"

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
std::string utils::request::_useragent = "";

void utils::request::init(){
    if (_p_guard)
    {
        return;
    } // already inited

    _p_guard = std::make_unique<_guard>();

    return;
}

int utils::request::get(
    const utils::request::request_param& __cr_param,
    utils::request::str_response* __op_res,
    std::string* __op_errmsg
){
    assert(_p_guard);

    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(curl_easy_init(), curl_easy_cleanup);
    if (!curl)
    {
        *__op_errmsg = "Failed to initialize curl object";
        return 1;
    }

    std::string url = _build_url(
        __cr_param.host, __cr_param.port, __cr_param.endpoint, __cr_param.https
    );
    std::string response_data;
    CURLcode res;

    CURL_SAFE_SETOPT(curl, CURLOPT_URL, url.c_str(), res, *__op_errmsg);
    CURL_SAFE_SETOPT(curl, CURLOPT_WRITEFUNCTION, _cb_curl_write_str, res, *__op_errmsg);
    CURL_SAFE_SETOPT(curl, CURLOPT_WRITEDATA, &response_data, res, *__op_errmsg);

    if (__cr_param.crt_verify)
    {
        CURL_SAFE_SETOPT(curl, CURLOPT_SSL_VERIFYPEER, 1L, res, *__op_errmsg);
        CURL_SAFE_SETOPT(curl, CURLOPT_SSL_VERIFYHOST, 2L, res, *__op_errmsg);

        if (!__cr_param.crt_path.empty())
        {
            CURL_SAFE_SETOPT(curl, CURLOPT_CAINFO, __cr_param.crt_path.c_str(), res, *__op_errmsg);
        } // custom ca / crt
    } // enable crt verify
    else
    {
        CURL_SAFE_SETOPT(curl, CURLOPT_SSL_VERIFYPEER, 0L, res, *__op_errmsg);
        CURL_SAFE_SETOPT(curl, CURLOPT_SSL_VERIFYHOST, 0L, res, *__op_errmsg);
    } // disable crt verify

    CURL_SAFE_SETOPT(curl, CURLOPT_TIMEOUT, __cr_param.timeout_s, res, *__op_errmsg);
    CURL_SAFE_SETOPT(curl, CURLOPT_FOLLOWLOCATION, 1L, res, *__op_errmsg);
    CURL_SAFE_SETOPT(curl, CURLOPT_USERAGENT, _get_useragent().c_str(), res, *__op_errmsg);

    res = curl_easy_perform(curl.get());
    if (res != CURLE_OK)
    {
        std::ostringstream oss;
        oss << "Failed to request: " << curl_easy_strerror(res);
        *__op_errmsg = oss.str();
        return 1;
    } // request failed

    long response_code = 0;
    curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &response_code);
    // should never fails

    *__op_res = {response_code, response_data};
    return 0;
}

int utils::request::get(
    const utils::request::request_param& __cr_param,
    utils::request::json_response* __op_res,
    std::string* __op_errmsg
){
    assert(_p_guard);

    str_response response = {};
    int ret = get(__cr_param, &response, __op_errmsg);
    if (ret != 0)
    {
        return ret;
    } // basic GET request failed

    Json::Value root;
    Json::CharReaderBuilder reader_builder;
    std::string json_errmsg;
    const std::unique_ptr<Json::CharReader> reader(reader_builder.newCharReader());
    if (!reader->parse(
        response.content.c_str(), response.content.c_str() + response.content.size(),
        &root, &json_errmsg
    ))
    {
        *__op_errmsg = "Failed to parse json: " + json_errmsg;
        return 1;
    } // json parse failed

    *__op_res = {response.response_code, root};
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
    if (_useragent.empty())
    {
        curl_version_info_data* ver = curl_version_info(CURLVERSION_NOW);
        // no need to free: curl_version_info() returns a reference of a static obj
        
        std::ostringstream oss;
        oss << "libcurl/" << ((ver->version_num >> 16) & 0xFF) << "."
                          << ((ver->version_num >> 8) & 0xFF) << "."
                          << (ver->version_num & 0xFF);

        _useragent = oss.str();
    } // build useragent if empty

    return _useragent;
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
