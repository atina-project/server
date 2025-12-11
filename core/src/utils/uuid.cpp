#include"core/utils/uuid.h"

#include"uuid/uuid.h"

using namespace atina::server::core;

bool utils::uuid::compare(const std::string& __c_s_lhs, const std::string& __c_s_rhs){
    uuid_t uuid_l, uuid_r;
    if (uuid_parse(__c_s_lhs.c_str(), uuid_l) != 0 || uuid_parse(__c_s_rhs.c_str(), uuid_r) != 0)
    {
        return false;
    }
    return uuid_compare(uuid_l, uuid_r) == 0;
}

std::string utils::uuid::generate(){
    uuid_t uuid;
    char uuid_str[37];
    uuid_generate(uuid);
    uuid_unparse_upper(uuid, uuid_str);
    return std::string(uuid_str);
}
