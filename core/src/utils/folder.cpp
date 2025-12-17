#include"core/utils/folder.h"

using namespace atina::server::core;
namespace fs = std::filesystem;

#define CREATE_FOLDER_IF_NOT_EXISTS(folder) \
    if (!fs::exists(folder))                \
        fs::create_directories(folder);

fs::path utils::folder::data(){
    fs::path data = "./data";
    // just for dev, will be changed later
    CREATE_FOLDER_IF_NOT_EXISTS(data);
    return data;
}

fs::path utils::folder::config(){
    fs::path config = data() / "config";
    CREATE_FOLDER_IF_NOT_EXISTS(config);
    return config;
}

fs::path utils::folder::email(){
    fs::path email = data() / "email";
    CREATE_FOLDER_IF_NOT_EXISTS(email);
    return email;
}

fs::path utils::folder::email_template(){
    fs::path email_template = email() / "template";
    CREATE_FOLDER_IF_NOT_EXISTS(email_template);
    return email_template;
}

fs::path utils::folder::log(){
    fs::path log = data() / "log";
    CREATE_FOLDER_IF_NOT_EXISTS(log);
    return log;
}

fs::path utils::folder::script(){
    fs::path script = data() / "script";
    CREATE_FOLDER_IF_NOT_EXISTS(script);
    return script;
}

fs::path utils::folder::temp(){
    fs::path temp = data() / "temp";
    CREATE_FOLDER_IF_NOT_EXISTS(temp);
    return temp;
}
