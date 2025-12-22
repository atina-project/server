#include"core/currency/converter_frankfurter.h"

#include<algorithm>
#include<ctime>
#include<sstream>
#include<time.h>

#include"json/json.h"
#include"sqlite_orm/sqlite_orm.h"

#include"core/utils/request.h"

using namespace atina::server::core;

static inline auto _create_storage(){
    using namespace atina::server::core::currency;
    using namespace sqlite_orm;

    return make_storage("./frankfurter.db",
        make_table("currency",
            make_column("code", &converter_frankfurter::currency_data::code, unique(), not_null())
        ),
        make_index("cidx_rate_time_ts_target", &converter_frankfurter::rate_data::time_ts, &converter_frankfurter::rate_data::target),
        make_table("rate",
            make_column("id", &converter_frankfurter::rate_data::id, primary_key().autoincrement()),
            make_column("time_ts", &converter_frankfurter::rate_data::time_ts, not_null()),
            make_column("target", &converter_frankfurter::rate_data::target, not_null()),
            make_column("rate", &converter_frankfurter::rate_data::rate, not_null()),
            make_column("extra", &converter_frankfurter::rate_data::extra, null()),
            foreign_key(&converter_frankfurter::rate_data::target).references(&converter_frankfurter::currency_data::code)
        )
    );
}

struct currency::converter_frankfurter::_impl {
    using storage_t = decltype(_create_storage());

    explicit _impl() : _storage(_create_storage()){
        this->_storage.sync_schema();

        std::vector<currency_data> currencies;
        for (const auto& it : converter_frankfurter::_supported_currencies)
        {
            currencies.push_back({ it });
        }

        try {
            auto guard = this->_storage.transaction_guard();

            this->_storage.remove_all<currency_data>();
            this->_storage.replace_range(currencies.begin(), currencies.end());

            guard.commit();
        }
        catch (const std::exception& e)
        {
            // TODO: handle exception
        }
        
        return; 
    }

    Json::Value get_latest_rate_data();
    Json::Value get_latest_rate_data(const std::string& __cr_base, const std::string& __cr_symbol);

    storage_t _storage;
}; // struct currency::converter_frankfurter::_impl

currency::converter_frankfurter::converter_frankfurter()
    : _p_impl(std::make_unique<currency::converter_frankfurter::_impl>())
{
    this->_refresh_latest_rate_data();
    return;
}

currency::converter_frankfurter::~converter_frankfurter() = default;

void currency::converter_frankfurter::init(){} // no special init

bool currency::converter_frankfurter::is_currency_supported(const std::string& __cr_ccode){
    return std::binary_search(
        this->_supported_currencies.begin(),
        this->_supported_currencies.end(),
        __cr_ccode
    );
}

double currency::converter_frankfurter::convert(double __amount, const std::string& __cr_from, const std::string& __cr_to){
    if (__amount < 0)
    {
        return -1;
    } // negative amount
    if (!this->is_currency_supported(__cr_from) || !this->is_currency_supported(__cr_to))
    {
        return -1;
    } // unsupported currency

    Json::Value root = this->_p_impl->get_latest_rate_data(__cr_from, __cr_to);
    if (root.empty())
    {
        return -1;
    } // get data failed

    double rate = root["rates"][__cr_to].asDouble();

    return __amount * rate;
}

double currency::converter_frankfurter::convert(
    uint64_t __time_ts, double __amount, const std::string& __cr_from, const std::string& __cr_to,
    uint64_t* __op_real_time_ts
){
    using namespace sqlite_orm;

    // TODO: allow getting data with endpoint /v1/%date%?base=%from$&symbols=%to%

    if (__amount < 0)
    {
        return -1;
    } // negative amount
    if (!this->is_currency_supported(__cr_from) || !this->is_currency_supported(__cr_to))
    {
        return -1;
    } // unsupported currency

    auto func_get_single_conversion_best_line = [this, __time_ts](
        const std::string& __cr_target
    ) -> rate_data {
        /**
         * SELECT * FROM rate 
         * WHERE target = ? AND time_ts < ?
         * ORDER BY time_ts DESC LIMIT 1;
         */
        auto prev_rows = this->_p_impl->_storage.get_all<rate_data>(
            where(c(&rate_data::target) == __cr_target &&
                  c(&rate_data::time_ts) < __time_ts),
            order_by(&rate_data::time_ts).desc(),
            limit(1)
        );
        /**
         * SELECT * FROM rate
         * WHERE target = ? AND time_ts >= ?
         * ORDER BY time_ts ASC LIMIT 1;
         */
        auto next_rows = this->_p_impl->_storage.get_all<rate_data>(
            where(c(&rate_data::target) == __cr_target &&
                  c(&rate_data::time_ts) >= __time_ts),
            order_by(&rate_data::time_ts).asc(),
            limit(1)
        );

        rate_data result;
        if (!prev_rows.empty() && !next_rows.empty())
        {
            uint64_t prev_diff = __time_ts - prev_rows[0].time_ts;
            uint64_t next_diff = next_rows[0].time_ts - __time_ts;
            result = (prev_diff < next_diff) ? prev_rows[0]
                                             : next_rows[0];
        }
        else if (!prev_rows.empty())
        {
            result = prev_rows[0];
        }
        else if (!next_rows.empty())
        {
            result = next_rows[0];
        }

        return { .id = -1 }; // got no data
    }; // auto func_get_single_conversion_best_line()

    if (__cr_from == this->_base_currency)
    {
        rate_data result = func_get_single_conversion_best_line(__cr_to);
        if (result.id == -1)
        {
            return -1;
        } // got no data

        *__op_real_time_ts = result.time_ts;
        return __amount * result.rate;
    } // from currency is base currency (euro)
    else if (__cr_to == this->_base_currency)
    {
        rate_data result = func_get_single_conversion_best_line(__cr_from);
        if (result.id == -1)
        {
            return -1;
        } // got no data

        *__op_real_time_ts = result.time_ts;
        return __amount / result.rate;
    } // to currency is base currency (euro)
    else
    {
        /**
         * SELECT time_ts FROM rate
         * WHERE target = ?
         * ORDER BY time_ts ASC;
         */
        auto func_select_all_ts_with_target_currency = [this](
            const std::string& __cr_target
        ) -> std::vector<uint64_t> {
            return this->_p_impl->_storage.select(&rate_data::time_ts,
                where(c(&rate_data::target) == __cr_target),
                order_by(&rate_data::time_ts).asc()
            );
        };

        auto from_tss = func_select_all_ts_with_target_currency(__cr_from);
        auto to_tss = func_select_all_ts_with_target_currency(__cr_to);
        std::vector<uint64_t> common_tss;
        std::set_intersection(
            from_tss.begin(), from_tss.end(),
            to_tss.begin(), to_tss.end(),
            std::back_inserter(common_tss)
        );
        // get all timestamps which have both from currency's & to currency's rate datas

        if (common_tss.empty())
        {
            return -1;
        } // no usable data

        auto it = std::lower_bound(common_tss.begin(), common_tss.end(), __time_ts);
        uint64_t best_ts;
        if (it == common_tss.end())
        {
            best_ts = common_tss.back();
        }
        else if (it == common_tss.begin())
        {
            best_ts = common_tss.front();
        }
        else
        {
            uint64_t prev_ts = *std::prev(it);
            uint64_t prev_diff = __time_ts - prev_ts;
            uint64_t next_ts = *it;
            uint64_t next_diff = next_ts - __time_ts;
            best_ts = (prev_diff < next_diff) ? prev_ts
                                              : next_ts;
        }
        // choose best ts

        /**
         * SELECT * FROM rate
         * WHERE time_ts = ? AND target = ?
         * LIMIT 1;
         */
        rate_data base_to_from_rate = this->_p_impl->_storage.get_all<rate_data>(
            where(c(&rate_data::time_ts) == best_ts &&
                  c(&rate_data::target) == __cr_from),
            limit(1)
        )[0];
        /**
         * SELECT * FROM rate
         * WHERE time_ts = ? AND target = ?
         * LIMIT 1;
         */
        rate_data base_to_to_rate = this->_p_impl->_storage.get_all<rate_data>(
            where(c(&rate_data::time_ts) == best_ts &&
                  c(&rate_data::target) == __cr_to),
            limit(1)
        )[0];

        *__op_real_time_ts = best_ts;
        return __amount / base_to_from_rate.rate * base_to_to_rate.rate;
    } // use base currency as conversion bridge

    return -1; // should never reach
}

int currency::converter_frankfurter::_refresh_latest_rate_data(){
    using namespace sqlite_orm;

    Json::Value root = this->_p_impl->get_latest_rate_data();
    if (root.empty())
    {
        return 1;
    } // get data failed

    int amount = root["amount"].asInt();
    std::string base = root["base"].asString();
    std::string date = root["date"].asString();
    Json::Value rates = root["rates"];

    if (base != this->_base_currency)
    {
        return 1;
    } // frankfurter uses euro as base currency

    std::tm time = {};
    std::istringstream iss(date);
    iss >> std::get_time(&time, "%Y-%m-%d");
    if (iss.fail())
    {
        return 1;
    } // parse date failed

    time.tm_hour = 15;
    time.tm_min = 0;
    time.tm_sec = 0;
    time.tm_isdst = 0;
    uint64_t rate_time_ts = timegm(&time);
    // Frankfurter updates its data every workday around 16:00 CET (15:00 UTC).
    // We need to check if datas we got before are *new*, therefore a timestamp
    // is built and we'll count the database before inserting new datas.
    // The time isn't exact, we don't know when exactly frankfurter refreshes
    // its data, but it isn't a big deal.

    bool latest_rate_data_already_inserted = this->_p_impl->_storage.count<rate_data>(
        where(c(&rate_data::time_ts) == rate_time_ts)
    ) > 0;
    if (latest_rate_data_already_inserted)
    {
        return 0;
    } // rate datas at this time found in db

    try {
        auto guard = this->_p_impl->_storage.transaction_guard();

        for (auto it = rates.begin(); it != rates.end(); ++it)
        {
            std::string key = it.name();
            double rate = it->asDouble() / amount;

            auto cmd = this->_p_impl->_storage.prepare(insert(rate_data{
                -1,
                rate_time_ts,
                key,
                rate,
                ""
            }));
            this->_p_impl->_storage.execute(cmd);
        }

        guard.commit();
    }
    catch (const std::exception& e)
    {
        // TODO: error handling
        return 1;
    }

    return 0;
}

Json::Value currency::converter_frankfurter::_impl::get_latest_rate_data(){
    utils::request::request_param param {
        .host = "api.frankfurter.dev",
        .endpoint = "/v1/latest"
    };
    utils::request::json_response response;
    std::string errmsg;
    int res = utils::request::get(param, &response, &errmsg);
    // TODO: support self-hosted frankfurter server in config
    if (res != 0)
    {
        return Json::Value();
    } // request failed
    if (response.response_code != 200)
    {
        return Json::Value();
    } // request not OK
    // TODO: error handling & log

    return response.root;
}

Json::Value currency::converter_frankfurter::_impl::get_latest_rate_data(
    const std::string& __cr_base, const std::string& __cr_symbol
){
    std::ostringstream oss;
    oss << "/v1/latest?base=" << __cr_base << "&symbols=" << __cr_symbol;
    // build endpoint

    utils::request::request_param param {
        .host = "api.frankfurter.dev",
        .endpoint = oss.str()
    };
    utils::request::json_response response;
    std::string errmsg;
    int res = utils::request::get(param, &response, &errmsg);
    if (res != 0)
    {
        return Json::Value();
    } // request failed
    if (response.response_code != 200)
    {
        return Json::Value();
    } // request not OK

    return response.root;
}
