#include<iostream>

#include"core/account.h"
#include"core/utils/crypto.h"

using namespace atina::server::core;

int main(){
    utils::crypto::init();

    account acc;

    acc.set_callback_create_admin_service_account(
        [](const std::string& __service_name, const std::string& __uuid, const std::string& __pswd){
            std::cout << "Admin service account doesn't exist, a new one has been created." << std::endl
                      << " - Service name: " << __service_name << std::endl
                      << " - Service UUID: " << __uuid << std::endl
                      << " - Service password: " << __pswd << std::endl
                      << "WARNING: password of this service account won't be shown again! Store it properly." << std::endl;
            return;
        }
    );
    acc.connect_db();

    return 0;
}
