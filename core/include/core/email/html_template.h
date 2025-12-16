#ifndef __ATINA_SERVER_CORE_EMAIL_HTML_TEMPLATE_H__
#define __ATINA_SERVER_CORE_EMAIL_HTML_TEMPLATE_H__

#include<string>
#include<unordered_map>
#include<variant>

namespace atina::server::core::email {

    class html_template final {

        public:
            html_template();
            html_template(const std::string& __cr_template);
            ~html_template();

            using contexts = std::unordered_map<std::string,std::variant<int, long, double, std::string>>;

        public:
            bool is_empty() const noexcept;
            std::string render(const contexts& __cr_ctx);

        private:
            std::string _template;

    }; // class html_template

} // namespace atina::server::core::email

#endif // __ATINA_SERVER_CORE_EMAIL_HTML_TEMPLATE_H__
