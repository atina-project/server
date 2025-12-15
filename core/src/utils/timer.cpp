#include"core/utils/timer.h"

using namespace atina::server::core;

utils::timer::timer(){
    this->_start = std::chrono::steady_clock::now();
    return;
}

void utils::timer::stop(){
    this->_stop = std::chrono::steady_clock::now();
    return;
}

int64_t utils::timer::count_s() const noexcept {
    return std::chrono::duration_cast<std::chrono::seconds>(
        this->_stop - this->_start
    ).count();
}

int64_t utils::timer::count_ms() const noexcept {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        this->_stop - this->_start
    ).count();
}

int64_t utils::timer::count_us() const noexcept {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        this->_stop - this->_start
    ).count();
}
