#pragma once

#ifndef __ATINA_SERVER_CORE_UTILS_TIME_H__
#define __ATINA_SERVER_CORE_UTILS_TIME_H__

#include<cstdint>
#include<string>

namespace atina::server::core::utils {

    class time {

        public:
            time(uint64_t __ts)
                : _ts(__ts){}

            std::string to_str() const;
            uint64_t to_ts(bool __no_ms = true) const noexcept;
            std::string to_utc_str() const;

        public:
            static time now();
            static int tz();  // get local time zone (min diff to UTC, e.g. UTC+8 -> 480)
            static std::string tz_str();

            friend bool operator<(time __lhs, time __rhs);
            friend bool operator<=(time __lhs, time __rhs);
            friend bool operator==(time __lhs, time __rhs);
            friend bool operator>(time __lhs, time __rhs);
            friend bool operator>=(time __lhs, time __rhs);

        private:
            uint64_t _ts;  // ms based utc timestamp

    }; // class time

    bool operator<(time __lhs, time __rhs);
    bool operator<=(time __lhs, time __rhs);
    bool operator==(time __lhs, time __rhs);
    bool operator>(time __lhs, time __rhs);
    bool operator>=(time __lhs, time __rhs);

} // namespace atina::server::core::utils

#endif // __ATINA_SERVER_CORE_UTILS_TIME_H__
