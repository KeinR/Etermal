#ifndef ETERMAL_SHELL_ERROR_H_INCLUDED
#define ETERMAL_SHELL_ERROR_H_INCLUDED

#include <exception>
#include <string>

namespace etm {
    
    /**
    * Describes an error that occurred in the Shell
    */
    class shellError: public std::exception {
    public:
        /// The location where the error occurred, local
        /// to the etm namespace
        std::string location;
        /// The error message
        std::string message;
        /// If the error is severe, that is, the raiser
        /// cannot contain/nullify its averse effects.
        bool severe;
        /**
        * Construct a shellError with args.
        * @param [in] location The location the error was detected
        * @param [in] message The error message
        * @param [in] severe If the error is @ref severe or not
        */
        shellError(const std::string &location, const std::string &message, bool severe) noexcept;
        /**
        * Gets the c_str value of @ref message
        * @return message.c_str()
        */
        const char *what() const noexcept override;
    };
}


#endif
