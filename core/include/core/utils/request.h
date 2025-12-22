#ifndef __ATINA_SERVER_CORE_UTILS_REQUEST_H__
#define __ATINA_SERVER_CORE_UTILS_REQUEST_H__

#include<cstddef>
#include<filesystem>
#include<memory>
#include<string>

#include"json/value.h"

namespace atina::server::core::utils {

    class request {

        public:
            struct request_param {
                std::string host;
                int port = -1;  // -1 means no specific
                std::string endpoint = "";
                bool https = true;
                int timeout_s = 10;
                bool crt_verify = true;
                std::filesystem::path crt_path = "";
            };

            struct str_response {
                long response_code;
                std::string content;
            }; // struct str_response

            struct json_response {
                long response_code;
                Json::Value root;
            }; // struct json_response

        public:
            /**
             * Initialize libcurl, create a guard project to call `curl_global_cleanup()` on exit.
             */
            static void init();

            /**
             * Perform GET request, parse response content as string.
             */
            static int get(
                const request_param& __cr_param,
                str_response* __op_res,
                std::string* __op_errmsg
            );

            /**
             * Perform GET request, parse response content as json.
             */
            static int get(
                const request_param& __cr_param,
                json_response* __op_res,
                std::string* __op_errmsg
            );

        private:
            struct _guard;
            static std::unique_ptr<_guard> _p_guard;
            static std::string _useragent;

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
