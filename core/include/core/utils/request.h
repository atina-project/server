#ifndef __ATINA_SERVER_CORE_UTILS_REQUEST_H__
#define __ATINA_SERVER_CORE_UTILS_REQUEST_H__

#include<cstddef>
#include<filesystem>
#include<memory>
#include<string>

namespace atina::server::core::utils {

    class request {

        public:
            struct response {
                long response_code;
                std::string content;
            }; // struct response

        public:
            static void init();
            static int get(
                const std::string& __cr_host,
                int __port,
                const std::string& __cr_endpoint,
                bool __https,
                int __timeout_s,
                response& __o_res,
                std::string& __o_errmsg,
                bool __crt_verify = true,
                const std::filesystem::path& __cr_crt_path = ""
            );

        private:
            struct _guard;
            static std::unique_ptr<_guard> _p_guard;

            static std::string _build_url(
                const std::string& __cr_host,
                int __port,
                const std::string& __cr_endpoint,
                bool __https
            );

            static std::string _get_useragent();

            static std::size_t _cb_curl_write_str(
                char* __p_data,
                std::size_t __size,
                std::size_t __nmemb,
                std::string* __p_userdata
            );

    }; // class request

} // namespace atina::server::core::utils

#endif // __ATINA_SERVER_CORE_UTILS_REQUEST_H__
