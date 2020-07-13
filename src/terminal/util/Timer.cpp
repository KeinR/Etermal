#include "Timer.h"

etm::Timer::Timer(): Timer(0) {
}
etm::Timer::Timer(int milliseconds) {
    setTime(milliseconds);
}
void etm::Timer::setTime(int milliseconds) {
    duration = std::chrono::milliseconds(milliseconds);
}
void etm::Timer::start() {
    end = std::chrono::steady_clock::now() + duration;
}
bool etm::Timer::hasEnded() {
    return std::chrono::steady_clock::now() >= end;
}
