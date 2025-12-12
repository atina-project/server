#pragma once

#ifndef __ATINA_SERVER_CORE_ACCOUNT_H__
#define __ATINA_SERVER_CORE_ACCOUNT_H__

#include<cstddef>
#include<functional>
#include<memory>
#include<string>

namespace atina::server::core {

    class account final {

        public:
            account();
            ~account();

        public:
            struct user_data {
                int id;                // also used as uid
                std::string username;  // username used for login (only latin chars allowed)
                std::string aka;       // non-essential also-known-as (all chars allowed)
                std::string email;
                std::string pswd_hash;
                std::size_t reg_ts;    // registration time (ts in seconds)
                bool verified;         // is email verified
                bool blocked;
                std::string extra;
            }; // struct user_data

            struct service_data {
                int id;
                std::string service_name; // service display name
                std::string uuid;         // service identifier
                std::string pswd_hash;    // pswd (should be auto generated)
                std::string descrip;
                std::size_t create_ts;    // creation time (ts in seconds)
                bool enabled;
                std::string extra;
            }; // struct service_data

            /**
             * Param: service name, uuid, password
             */
            using cb_create_admin_service_account = std::function<void(
                const std::string&, const std::string&, const std::string&
            )>;

        public:
            /**
             * Initalize internal implementation and connect to database.
             */
            void connect_db();

            /**
             * Set callback on creating admin service account.
             * Call it before calling `connect_db()`.
             */
            void set_callback_create_admin_service_account(cb_create_admin_service_account __cb);

        private:
            struct _impl;
            std::unique_ptr<_impl> _p_impl;

            cb_create_admin_service_account _cb_deferred_create_admin_service_account;

    }; // class account

} // namespace atina::server::core

#endif // __ATINA_SERVER_CORE_ACCOUNT_H__
