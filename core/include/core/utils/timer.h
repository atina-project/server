#pragma once

#ifndef __ATINA_SERVER_CORE_UTILS_TIMER_H__
#define __ATINA_SERVER_CORE_UTILS_TIMER_H__

#include<chrono>
#include<cstdint>

namespace atina::server::core::utils {

    class timer {

        public:
            timer();
            ~timer(){}

            void stop();

            int64_t count_s() const noexcept;
            int64_t count_ms() const noexcept;
            int64_t count_us() const noexcept;

        private:
            using timepoint_t = decltype(std::chrono::steady_clock::now());

            timepoint_t _start;
            timepoint_t _stop;

    }; // class timer

} // namespace atina::server::core::utils

#endif // __ATINA_SERVER_CORE_UTILS_TIMER_H__
