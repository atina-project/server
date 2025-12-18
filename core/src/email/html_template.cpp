#include"core/email/html_template.h"

#include<cstddef>
#include<filesystem>
#include<fstream>
#include<sstream>

#include"core/utils/folder.h"

using namespace atina::server::core;
namespace fs = std::filesystem;

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
        } // replace all tags
    }

    return out;
}

email::html_template email::html_template::create(const std::string& __cr_name, email::html_template::lang __lang){
    fs::path template_path = utils::folder::email_template();
    std::string filename = __cr_name + "_";
    switch (__lang)
    {
        case lang::en: filename += "en"; break;
        case lang::zhCN: filename += "zhCN"; break;
        default: return html_template();
        // shouldn't happen, but if an illegal lang is provided,
        // return an empty template
    }
    filename += ".html";
    // file ext
    template_path /= filename;

    if (!fs::exists(template_path))
    {
        if (__lang == lang::en)
        {
            return html_template();
        } // english version doesn't exist, no need to fallback

        fs::path fallback_template_path = utils::folder::email_template() / (__cr_name + "_en.html");
        // fallback to english version
        if (!fs::exists(fallback_template_path))
        {
            return html_template();
        } // fallback template doesn't exist
    } // template doesn't exist

    std::ifstream template_file(template_path);
    if (!template_file.is_open())
    {
        return html_template();
    }

    std::ostringstream oss;
    oss << template_file.rdbuf();
    std::string content = oss.str();
    template_file.close();

    return html_template(content);
}
