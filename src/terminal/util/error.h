#ifndef ETERMAL_TERMINAL_ERROR_H_INCLUDED
#define ETERMAL_TERMINAL_ERROR_H_INCLUDED

#include <exception>
#include <string>

/*
* Fontend error, thrown when rendering terminal text, etc
*/

namespace etm {
    class fe_error: public std::exception {
        const std::string message;
    public:
        fe_error(const std::string &message) noexcept;
        const char *what() const noexcept override;
    };
}


#endif
