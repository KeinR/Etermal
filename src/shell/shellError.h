#ifndef ETERMAL_SHELL_ERROR_H_INCLUDED
#define ETERMAL_SHELL_ERROR_H_INCLUDED

#include <exception>
#include <string>

/*
* Fontend error, thrown when rendering terminal text, etc
*/

namespace etm {
    class shellError: public std::exception {
    public:
        // The location where the error occurred, local
        // to the etm namespace
        std::string location;
        // The error message
        std::string message;
        // If the error is severe, that is, the raiser
        // cannot contain/nullify its averse effects
        bool severe;
        shellError(const std::string &location, const std::string &message, bool severe) noexcept;
        const char *what() const noexcept override;
    };
}


#endif
