#ifndef ETERMAL_TIMER_H_INCLUDED
#define ETERMAL_TIMER_H_INCLUDED

#include <chrono>

namespace etm {
    class Timer {
        std::chrono::milliseconds duration;
        std::chrono::steady_clock::time_point end;
    public:
        // Sets duration to zero
        Timer();
        Timer(int milliseconds);
        void setTime(int milliseconds);
        void start();
        bool hasEnded();
    };
}

#endif
