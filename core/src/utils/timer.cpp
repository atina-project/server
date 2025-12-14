#include"core/utils/timer.h"

using namespace atina::server::core;

utils::timer::timer(){
    this->_start = std::chrono::steady_clock::now();
    return;
}

void utils::timer::stop(){
    auto now = std::chrono::steady_clock::now();

    this->_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - this->_start
    ).count();

    return;
}

uint64_t utils::timer::count() const noexcept {
    return this->_duration;
}
