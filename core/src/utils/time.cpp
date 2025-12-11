#include"core/utils/time.h"

#include<chrono>
#include<cstdlib>
#include<ctime>
#include<iomanip>
#include<sstream>

using namespace atina::server::core;

std::string utils::time::to_str() const {
    time_t s = this->_ts / 1000;
    int ms = this->_ts % 1000;
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&s), "%Y-%m-%d %H:%M:%S")
        << '.' << std::setw(3) << std::setfill('0') << ms;
    return oss.str();
}

uint64_t utils::time::to_ts(bool __no_ms) const noexcept {
    return __no_ms ? this->_ts / 1000 : this->_ts;
}

std::string utils::time::to_utc_str() const {
    time_t s = this->_ts / 1000;
    int ms = this->_ts % 1000;
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&s), "%Y-%m-%d %H:%M:%S")
        << '.' << std::setw(3) << std::setfill('0') << ms;
    return oss.str();
}

utils::time utils::time::now(){
    auto now = std::chrono::system_clock::now();
    auto ts_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    return time(ts_ms);
}

int utils::time::tz(){
    auto now = std::chrono::system_clock::now();
    std::time_t ts_s = std::chrono::system_clock::to_time_t(now);
    std::tm* local = std::localtime(&ts_s);
    int offset = local->tm_gmtoff;
    return offset / 60;
}

std::string utils::time::tz_str(){
    int offset_min = tz();
    if (offset_min == 0)
    {
        return "UTC";
    }

    std::ostringstream oss;
    oss << "UTC" << (offset_min > 0 ? "+" : "-") << std::abs(offset_min / 60);
    if (offset_min % 60 != 0)
    {
        oss << std::setprecision(1) << offset_min / 60.0;
    }

    return oss.str();
}

bool utils::operator<(utils::time __lhs, utils::time __rhs){
    return __lhs._ts < __rhs._ts;
}

bool utils::operator<=(utils::time __lhs, utils::time __rhs){
    return __lhs._ts <= __rhs._ts;
}

bool utils::operator==(utils::time __lhs, utils::time __rhs){
    return __lhs._ts == __rhs._ts;
}

bool utils::operator>(utils::time __lhs, utils::time __rhs){
    return __lhs._ts > __rhs._ts;
}

bool utils::operator>=(utils::time __lhs, utils::time __rhs){
    return __lhs._ts >= __rhs._ts;
}
