#ifndef __ATINA_SERVER_CORE_UTILS_TIMER_H__
#define __ATINA_SERVER_CORE_UTILS_TIMER_H__

#include<chrono>
#include<cstdint>

namespace atina::server::core::utils {

    class timer {

        public:
            /**
             * Create and start a timer.
             */
            timer();
            ~timer(){}

            /**
             * Stop the timer and count duration.
             */
            void stop();
            /**
             * Get duration in milliseconds.
             */
            uint64_t count() const noexcept;

        private:
            using now_t = decltype(std::chrono::steady_clock::now());

            now_t _start;
            uint64_t _duration;

    }; // class timer

} // namespace atina::server::core::utils

#endif // __ATINA_SERVER_CORE_UTILS_TIMER_H__
