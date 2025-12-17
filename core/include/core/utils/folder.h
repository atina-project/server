#pragma once

#ifndef __ATINA_SERVER_CORE_UTILS_FOLDER_H__
#define __ATINA_SERVER_CORE_UTILS_FOLDER_H__

#include<filesystem>

namespace atina::server::core::utils {

    class folder {

        public:
            static std::filesystem::path data();
            static std::filesystem::path config();
            static std::filesystem::path email();
            static std::filesystem::path email_template();
            static std::filesystem::path log();
            static std::filesystem::path script();
            static std::filesystem::path temp();

    }; // class folder

} // namespace atina::server::core::utils

#endif // __ATINA_SERVER_CORE_UTILS_FOLDER_H__
