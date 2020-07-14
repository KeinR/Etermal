#ifndef ETERMAL_SHELL_ERROR_H_INCLUDED
#define ETERMAL_SHELL_ERROR_H_INCLUDED

#include <exception>
#include <string>

/*
* Fontend error, thrown when rendering terminal text, etc
*/

namespace etm {
    class be_error: public std::exception {
        const std::string message;
    public:
        be_error(const std::string &message) noexcept;
        const char *what() const noexcept override;
    };
}


#endif
