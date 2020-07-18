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
        // that affects the entire terminal, that the
        // function that raised the error cannot contain
        // negative side effects.
        // Non-severe errors are ones that, for the most part,
        // are contained by the raiser, allowing the normal function
        // of the rest of the code.
        bool severe;
        termError(const std::string &location, const std::string &message, int code, bool severe) noexcept;
        const char *what() const noexcept override;
    };
}


#endif
