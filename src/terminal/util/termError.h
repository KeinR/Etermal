#ifndef ETERMAL_TERMINAL_ERROR_H_INCLUDED
#define ETERMAL_TERMINAL_ERROR_H_INCLUDED

#include <exception>
#include <string>

namespace etm {

    /**
    * Contains information about an error that was encountered
    * by the @ref Terminal.
    */
    class termError: public std::exception {
    public:
        /// The location within the etm namespace
        /// that the error was detected.
        std::string location;
        /// The error message
        std::string message;
        /// The error code.
        /// Set to zero when not applicable.
        int code;
        /// If the error is severe or not.
        /// Severe errors indicate there's a problem
        /// that affects the entire terminal, that the
        /// function that raised the error cannot contain
        /// negative side effects.
        /// Non-severe errors are ones that, for the most part,
        /// are contained by the raiser, allowing the rest of the
        /// code to function as normal.
        bool severe;

        /**
        * Construct a new termError, setting @ref code and @ref severe
        * to 0 and false, respectively.
        */
        termError();

        /**
        * Construct a new termError with arguments.
        * @param [in] location The location at which the error was detected.
        * @param [in] message The error message
        * @param [in] code The error @ref code
        * @param [in] severe Whether the error is @ref severe or not.
        */
        termError(const std::string &location, const std::string &message, int code, bool severe) noexcept;

        /**
        * Returns the c_str() of @ref message.
        * @note The lifetime of the returned c-string is tied to that
        * of @ref message, and by extension the entire object.
        * @return @ref message
        */
        const char *what() const noexcept override;
    };
}


#endif
