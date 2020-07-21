#ifndef ETERMAL_TIMER_H_INCLUDED
#define ETERMAL_TIMER_H_INCLUDED

#include <chrono>

namespace etm {

    /**
    * Measures a length of time.
    */
    class Timer {
        /// The time to wait.
        /// @see setTime(int milliseconds)
        std::chrono::milliseconds duration;
        /// The end-time (when the duration is up)
        /// @see start()
        /// @see hasEnded()
        std::chrono::steady_clock::time_point end;
    public:
        // Sets duration to zero
        /**
        * Creates a Timer with the @ref duration set to zero milliseconds.
        */
        Timer();
        /**
        * Creates a Timer with duration.
        * @param [in] milliseconds The duration of the timer, in milliseconds
        */
        Timer(int milliseconds);
        /**
        * Sets the timer duration.
        * @param [in] milliseconds The duration of the timer, in milliseconds
        * @see duration
        */
        void setTime(int milliseconds);
        /**
        * Start the timer, setting the @ref end to now + @ref duration.
        * @see hasEnded()
        */
        void start();
        /**
        * Check if the timer has ended, that is,
        * if "now" is greater than @ref end.
        * @return `true` if the timer has ended
        */
        bool hasEnded();
    };
}

#endif
