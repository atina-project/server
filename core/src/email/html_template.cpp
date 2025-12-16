#include"core/email/html_template.h"

#include<cstddef>
#include<sstream>

using namespace atina::server::core;

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

email::html_template::html_template(){}

email::html_template::html_template(const std::string& __cr_template)
    : _template(__cr_template){}

email::html_template::~html_template() = default;

bool email::html_template::is_empty() const noexcept {
    return this->_template.empty();
}

std::string email::html_template::render(const email::html_template::contexts& __cr_ctx){
    if (this->_template.empty())
    {
        return "";
    } // empty template

    std::string out = this->_template;

    for (const auto& ctx : __cr_ctx)
    {
        std::string tag = "%" + ctx.first + "%";
        if (out.find(tag) == std::string::npos)
        {
            continue;
        } // tag doesn't exist in current out

        std::string value = std::visit(overloaded{
            [](int __arg){ return std::to_string(__arg); },
            [](long __arg){ return std::to_string(__arg); },
            [](double __arg){ return (std::ostringstream() << __arg).str(); },
            // std::to_string() has fixed precision
            [](const std::string& __cr_arg){ return __cr_arg; }
        }, ctx.second);
        std::size_t bpos;
        std::size_t len = tag.size();
        while ((bpos = out.find(tag)) != std::string::npos)
        {
            out.replace(bpos, len, value);
        }
    }

    return out;
}
