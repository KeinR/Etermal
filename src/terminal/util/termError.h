#ifndef ETERMAL_TERMINAL_ERROR_H_INCLUDED
#define ETERMAL_TERMINAL_ERROR_H_INCLUDED

#include <exception>
#include <string>

/*
* Fontend error, thrown when rendering terminal text, etc
*/

namespace etm {
    class termError: public std::exception {
    public:
        // The location within the etm namespace
        // that the error was detected.
        std::string location;
        // The error message
        std::string message;
        // The error code.
        // Set to zero when not applicable.
        int code;
        // If the error is severe or not.
        // Severe errors indicate there's a problem
        // that affects the entire terminal, and
        // as a result the terminal may be unstable.
        // low severity errors, while bad, can be ignored
        // if you have to.
        bool severe;
        termError(const std::string &location, const std::string &message, int code, bool severe) noexcept;
        const char *what() const noexcept override;
    };
}


#endif
