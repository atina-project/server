#include"core/account.h"

#include<cassert>

#include"core/utils/crypto.h"
#include"core/utils/time.h"
#include"core/utils/uuid.h"

#include"sqlite_orm/sqlite_orm.h"

using namespace atina::server::core;

static inline auto _create_storage(){
    using namespace sqlite_orm;

    return make_storage("./account.db",
        make_index("idx_user_account_aka", &account::user_data::aka),
        make_table("user_account",
            make_column("id", &account::user_data::id, primary_key().autoincrement()),
            make_column("username", &account::user_data::username, unique(), not_null()),
            make_column("aka", &account::user_data::aka, null()),
            make_column("email", &account::user_data::email, unique(), not_null()),
            make_column("pswd_hash", &account::user_data::pswd_hash, not_null()),
            make_column("reg_ts", &account::user_data::reg_ts, not_null()),
            make_column("verified", &account::user_data::verified, not_null()),
            make_column("blocked", &account::user_data::blocked, not_null()),
            make_column("extra", &account::user_data::extra, null())
        ),
        make_table("service_account",
            make_column("id", &account::service_data::id, primary_key().autoincrement()),
            make_column("service_name", &account::service_data::service_name, unique(), not_null()),
            make_column("uuid", &account::service_data::uuid, unique(), not_null()),
            make_column("pswd_hash", &account::service_data::pswd_hash, not_null()),
            make_column("descrip", &account::service_data::descrip, null()),
            make_column("create_ts", &account::service_data::create_ts, not_null()),
            make_column("enabled", &account::service_data::enabled, not_null()),
            make_column("extra", &account::service_data::extra, null())
        )
    );
}

struct account::_impl {
    using storage_t = decltype(_create_storage());

    explicit _impl(cb_create_admin_service_account __cb_casa)
        : _storage(_create_storage()),
          _cb_create_admin_service_account(std::move(__cb_casa)){
        this->_storage.sync_schema();
        return;
    }

    int create_admin_service_account();
    
    storage_t _storage;
    cb_create_admin_service_account _cb_create_admin_service_account;
}; // struct account::_impl

account::account() : _p_impl(nullptr){}

account::~account() = default;

void account::connect_db(){
    using namespace sqlite_orm;

    if (this->_p_impl)
    {
        return;
    } // impl already inited, db connected

    this->_p_impl = std::make_unique<_impl>(
        std::move(this->_cb_deferred_create_admin_service_account)
    );

    bool admin_service_exists = this->_p_impl->_storage.count<service_data>(
        where(c(&service_data::service_name) == "admin")
    ) > 0;
    if (!admin_service_exists)
    {
        this->_p_impl->create_admin_service_account();
    }

    return;
}

void account::set_callback_create_admin_service_account(account::cb_create_admin_service_account __cb){
    if (!(this->_p_impl))
    {
        this->_cb_deferred_create_admin_service_account = std::move(__cb);
    }
    else
    {
        this->_p_impl->_cb_create_admin_service_account = std::move(__cb);
    }
    return;
}

int account::_impl::create_admin_service_account(){
    assert(utils::crypto::is_init());

    std::string uuid = utils::uuid::generate();
    std::string pswd = utils::crypto::get_random_str(8);
    std::string pswd_hash = utils::crypto::get_pswd_hash(pswd, false);
    // do not overwrite pswd mem for now

    this->_storage.insert(service_data{
        -1,
        "admin",
        uuid,
        pswd_hash,
        "Atina server admin service",
        utils::time::now().to_ts(),
        true,
        ""
    });
    // if failed to create admin service account: let it crash
    this->_cb_create_admin_service_account("admin", uuid, pswd);

    utils::crypto::memzero(pswd);
    return 0;
}
