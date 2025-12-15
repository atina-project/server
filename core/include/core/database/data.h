#pragma once

#ifndef __ATINA_SERVER_CORE_DATABASE_DATA_H__
#define __ATINA_SERVER_CORE_DATABASE_DATA_H__

#include<cstdint>
#include<functional>
#include<memory>
#include<string>

namespace atina::server::core::database {

    class data final {

        public:
            data();
            ~data();

            /**
             * Param: service name, uuid, pswd
             */
            using cb_pre_create_service = std::function<void(
                const std::string&, const std::string&, const std::string&
            )>;

            /**
             * Param: id, service name, uuid, pswd, description, enabled
             */
            using cb_post_create_service = std::function<void(
                int, const std::string&, const std::string&, const std::string&,
                const std::string&, bool
            )>;

            /**
             * Param: service name, errmsg
             */
            using cb_err_create_service = std::function<void(
                const std::string&, const std::string&
            )>;

        public:
            void init();
            bool is_init();

            int create_user();
            int create_service(
                const std::string& __cr_service_name, const std::string& __cr_descrip, bool __enabled,
                cb_pre_create_service __cb_pre, cb_post_create_service __cb_post, cb_err_create_service __cb_err,
                bool __no_pswd = false
            );

            void set_cb_post_create_admin_service(cb_post_create_service __cb);

        public:
            struct user_data {
                int id;                // also used as uid
                std::string username;  // username used for login (only latin chars allowed)
                std::string aka;       // non-essential also-known-as (all chars allowed)
                std::string email;
                std::string pswd_hash;
                uint64_t reg_ts;       // registration time (ts in seconds)
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
                uint64_t create_ts;       // creation time (ts in seconds)
                bool enabled;
                std::string extra;
            }; // struct service_data

            struct audit_data {
                int id;
                uint64_t time_ts_ms;
                std::string service_uuid;
                std::string table_name;
                std::string action;
                int record_id;
                std::string record_col_name;
                std::string msg;
                std::string cmd;
                bool success;
                int64_t duration_us;
                std::string extra;
            }; // struct audit_data

        private:
            struct _impl;
            std::unique_ptr<_impl> _p_impl;

            std::string _internal_service_uuid;

            cb_post_create_service _cb_post_create_admin_service;

    }; // class data

} // namespace atina::server::core::database

#endif // __ATINA_SERVER_CORE_DATABASE_DATA_H__
