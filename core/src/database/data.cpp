#include"core/database/data.h"

#include<cassert>
#include<exception>
#include<sstream>

#include"core/utils/crypto.h"
#include"core/utils/time.h"
#include"core/utils/timer.h"
#include"core/utils/uuid.h"

#include"sqlite_orm/sqlite_orm.h"

using namespace atina::server::core;

#define ACTION_INSERT "INSERT"

#define INSERT_AUDIT_MSG(uuid, table, action, record_id, record_col, msg, cmd, duration)    \
    this->_p_impl->_storage.insert(audit_data{                                              \
        -1,                                                                                 \
        atina::server::core::utils::time::now().to_ts(false),                               \
        uuid, table, action, record_id, record_col,                                         \
        msg, cmd, true, duration, ""                                                        \
    })

#define INSERT_FAILED_AUDIT_MSG(uuid, table, action, record_id, record_col, msg, cmd)       \
    this->_p_impl->_storage.insert(audit_data{                                              \
        -1,                                                                                 \
        atina::server::core::utils::time::now().to_ts(false),                               \
        uuid, table, action, record_id, record_col,                                         \
        msg, cmd, false, -1, ""                                                             \
    })

static inline auto _create_storage(){
    using namespace atina::server::core::database;
    using namespace sqlite_orm;

    return make_storage("./data.db",
        make_index("idx_user_aka", &data::user_data::aka),
        make_table("user",
            make_column("id", &data::user_data::id, primary_key().autoincrement()),
            make_column("username", &data::user_data::username, unique(), not_null()),
            make_column("aka", &data::user_data::aka, null()),
            make_column("email", &data::user_data::email, unique(), not_null()),
            make_column("pswd_hash", &data::user_data::pswd_hash, not_null()),
            make_column("reg_ts", &data::user_data::reg_ts, not_null()),
            make_column("verified", &data::user_data::verified, not_null()),
            make_column("blocked", &data::user_data::blocked, not_null()),
            make_column("extra", &data::user_data::extra, null())
        ),
        make_table("service",
            make_column("id", &data::service_data::id, primary_key().autoincrement()),
            make_column("service_name", &data::service_data::service_name, unique(), not_null()),
            make_column("uuid", &data::service_data::uuid, unique(), not_null()),
            make_column("pswd_hash", &data::service_data::pswd_hash, not_null()),
            make_column("descrip", &data::service_data::descrip, null()),
            make_column("create_ts", &data::service_data::create_ts, not_null()),
            make_column("enabled", &data::service_data::enabled, not_null()),
            make_column("extra", &data::service_data::extra, null())
        ),
        make_index("idx_audit_service_uuid", &data::audit_data::service_uuid),
        make_index("idx_audit_table_name", &data::audit_data::table_name),
        make_index("cidx_audit_table_name_action", &data::audit_data::table_name, &data::audit_data::action),
        make_index("cidx_audit_table_name_action_record_col_name", &data::audit_data::table_name, &data::audit_data::action, &data::audit_data::record_col_name),
        make_index("idx_audit_success", &data::audit_data::success),
        make_table("audit",
            make_column("id", &data::audit_data::id, primary_key().autoincrement()),
            make_column("time_ts_ms", &data::audit_data::time_ts_ms, not_null()),
            make_column("service_uuid", &data::audit_data::service_uuid, not_null()),
            make_column("table_name", &data::audit_data::table_name, not_null()),
            make_column("action", &data::audit_data::action, not_null()),
            make_column("record_id", &data::audit_data::record_id, not_null()),
            make_column("record_col_name", &data::audit_data::record_col_name, null()),
            make_column("msg", &data::audit_data::msg, null()),
            make_column("cmd", &data::audit_data::cmd, null()),
            make_column("success", &data::audit_data::success, not_null()),
            make_column("duration_us", &data::audit_data::duration_us, null()),
            make_column("extra", &data::audit_data::extra, null()),
            foreign_key(&data::audit_data::service_uuid).references(&data::service_data::uuid)
        )
    );
}

struct database::data::_impl {
    using storage_t = decltype(_create_storage());

    explicit _impl() : _storage(_create_storage()){
        this->_storage.sync_schema();
        return;
    }

    storage_t _storage;

}; // database::data::_impl

database::data::data() : _p_impl(nullptr){}

database::data::~data() = default;

void database::data::init(){
    using namespace sqlite_orm;

    if (this->_p_impl)
    {
        return;
    } // already inited

    this->_p_impl = std::make_unique<_impl>();

    bool internal_service_exists = this->_p_impl->_storage.count<service_data>(
        where(c(&service_data::service_name) == "internal")
    ) > 0;
    bool admin_service_exists = this->_p_impl->_storage.count<service_data>(
        where(c(&service_data::service_name) == "admin")
    ) > 0;

    if (!internal_service_exists)
    {
        this->create_service(
            "internal", "Atina server internal service", true,
            [this](const std::string&, const std::string& __uuid, const std::string&){
                this->_internal_service_uuid = __uuid;
                return;
            }, nullptr, nullptr, true
        );
    } // internal service not exist
    else
    {
        this->_internal_service_uuid = this->_p_impl->_storage.get_all<service_data>(
            where(c(&service_data::service_name) == "internal"),
            limit(1)
        )[0].uuid;
    } // internal service exists, get uuid

    if (!admin_service_exists)
    {
        this->create_service(
            "admin", "Atina server admin service", true,
            nullptr, this->_cb_post_create_admin_service, nullptr
        );
    }

    return;
}

bool database::data::is_init(){
    return this->_p_impl != nullptr;
}

int database::data::create_user(){return 0;}

int database::data::create_service(
    const std::string& __cr_service_name, const std::string& __cr_descrip, bool __enabled,
    database::data::cb_pre_create_service __cb_pre,
    database::data::cb_post_create_service __cb_post,
    database::data::cb_err_create_service __cb_err,
    bool __no_pswd)
{
    using namespace sqlite_orm;

    std::string uuid = utils::uuid::generate();
    std::string pswd;
    std::string pswd_hash = "";
    if (!__no_pswd)
    {
        pswd = utils::crypto::get_random_str(8);
        // default pswd len, may be changed later
        pswd_hash = utils::crypto::get_pswd_hash(pswd, false);
    } // service has pswd

    if (__cb_pre != nullptr)
    {
        __cb_pre(__cr_service_name, uuid, pswd);
    } // pre create callback

    auto cmd = this->_p_impl->_storage.prepare(insert(service_data{
        -1,
        __cr_service_name,
        uuid,
        pswd_hash,
        __cr_descrip,
        utils::time::now().to_ts(),
        __enabled,
        ""
    }));
    std::string cmd_str = cmd.sql();

    try {
        auto guard = this->_p_impl->_storage.transaction_guard();

        utils::timer timer;
        int inserted_id = this->_p_impl->_storage.execute(cmd);
        timer.stop();

        INSERT_AUDIT_MSG(
            this->_internal_service_uuid, "service", ACTION_INSERT,
            inserted_id, "",
            std::string("Create service: ").append(__cr_service_name),
            cmd_str, timer.count_us()
        );

        if (__cb_post != nullptr)
        {
            __cb_post(
                inserted_id, __cr_service_name, uuid, pswd,
                __cr_descrip, __enabled
            );
        } // post create callback

        guard.commit();
    }
    catch (const std::exception& e)
    {
        std::string errmsg = e.what();
        std::ostringstream oss;
        oss << "Failed create service: " << __cr_service_name
            << " [errmsg: \"" << errmsg << "\"]";

        INSERT_FAILED_AUDIT_MSG(
            this->_internal_service_uuid, "service", ACTION_INSERT,
            -1, "",
            oss.str(), cmd_str
        );

        if (__cb_err != nullptr)
        {
            __cb_err(__cr_service_name, errmsg);
        } // err create callback

        return 1;
    }

    if (!__no_pswd)
    {
        utils::crypto::memzero(pswd);
    }
    return 0;
}

void database::data::set_cb_post_create_admin_service(database::data::cb_post_create_service __cb){
    this->_cb_post_create_admin_service = std::move(__cb);
    return;
}
