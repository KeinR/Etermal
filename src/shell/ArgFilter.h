#ifndef ETERMAL_ARGFILTER_H_INCLUDED
#define ETERMAL_ARGFILTER_H_INCLUDED

#include <string>
#include <vector>

#include "Args.h"

namespace etm {

    /**
    * Used to process - or filter, if you will -
    * command arguments.
    * @see Args
    * @see data
    * @see Shell
    */
    class ArgFilter {
    public:

        /**
        * Possible datatypes for an argument value
        */
        enum datatype {
            /// String type
            STRING,
            /// Integer type
            INT,
            /// Floating point type
            FLOAT,
            /// Boolean type
            BOOL
        };

        /**
        * Converts a @ref datatype to its string representation.
        * Returns "[invalid enum]" upon failure.
        * @param [in] val The datatype
        * @return A c-string representing the datatype.
        */
        static const char *datatypeToString(datatype val);

        /**
        * A single filter consisting of a parameter name
        * and its desired parameter type.
        */
        struct Filter {
            /// The parameter name
            std::string name;
            /// The desired parameter type
            datatype type;
            /**
            * Construct a default-initialized Filter.
            * @ref type is set to datatype::STRING
            */
            Filter();
            /**
            * Construct a Filter with arguments.
            * @param [in] name The parameter name
            * @param [in] type The desired paramter type
            */
            Filter(const std::string &name, datatype type);
        };

        /// Type of filter container
        typedef std::vector<Filter> filters_t;
        /// Type used to represent a sequence of command aliases
        typedef std::map<std::string, std::vector<Filter>::size_type> aliases_t;

        /**
        * Used to determine what happens when
        * there was a problem when processing
        * arguments.
        * This is typically something trivial like
        * a bad parameter type, however not so much
        * if it's a @ref internalError.
        * @see DefaultErrorHandle
        * @see setErrorHandle(ErrorHandle &errorHandle)
        */
        class ErrorHandle {
        public:
            virtual ~ErrorHandle() = 0;

            // `position` is the argument index at which the error was
            // detected, including parameters.
            // The strings returned by all the functions are the
            // messages displayed to the user.
            // They can be empty if that's what you want.

            /**
            * Called when the data cannot be parsed to the expected datatype.
            * @param [in] position The argument position (with 0 being the invokation)
            * @param [in] data The parameter given
            * @param [in] expectedType The type `data` was expected to be
            * @return String to display to the user
            */
            virtual std::string badDatatype(int position, const std::string &data, datatype expectedType) = 0;
            /**
            * Called when the flag was configured to accept a parameter, but instead hit
            * the end of the commands list.
            * @param [in] position The argument position (with 0 being the invokation)
            * @param [in] lastFlag The last flag, the one that the parameter was for
            * @param [in] expectedType The type `data` was expected to be
            * @return String to display to the user
            */
            virtual std::string noParam(int position, const std::string &lastFlag, datatype expectedType) = 0;
            /**
            * Called when the given flag does not exist
            * @param [in] position The argument position (with 0 being the invokation)
            * @param [in] badFlag The flag
            * @return String to display to the user
            */
            virtual std::string badFlag(int position, const std::string &badFlag) = 0;
            /**
            * Called when there are too many array arguments
            * @param [in] position The argument position (with 0 being the invokation)
            * @param [in] int The max allowed args
            * @return String to display to the user
            */
            virtual std::string tooManyArgs(int position, unsigned int max) = 0;
            /**
            * Called when some internal error occurred.
            * This should never happen, but it's good to have some
            * way to handle it if it does.
            * @param [in] position The argument position (with 0 being the invokation)
            * @param [in] errMsg The error message
            * @return String to display to the user
            */
            virtual std::string internalError(int position, const std::string &errMsg) = 0;
            /**
            * Check if the ArgFilter should stop processing the commands
            * as soon as it encounters and error.
            * @return `true` if yes
            */
            virtual bool doFailfast() = 0;
            /**
            * Check if the Shell should not execute the
            * command if there was an error parsing the commands.
            * @return `true` if yes
            */
            virtual bool doFail() = 0;
        };

        /**
        * The default error handle.
        * @see getStoreDefaultErrorHandle()
        * @see setDefaultErrorHandle(ErrorHandle &handle)
        */
        class DefaultErrorHandle: public ErrorHandle {
        public:
            /**
            * Construct a new object.
            */
            DefaultErrorHandle();
            std::string badDatatype(int position, const std::string &data, datatype expectedType) override;
            std::string noParam(int position, const std::string &lastFlag, datatype expectedType) override;
            std::string badFlag(int position, const std::string &badFlag) override;
            std::string tooManyArgs(int position, unsigned int max) override;
            std::string internalError(int position, const std::string &errMsg) override;
            /**
            * Check if the ArgFilter should stop processing the commands
            * as soon as it encounters and error.
            * @return true
            */
            bool doFailfast() override;
            /**
            * Check if the Shell should not execute the
            * command if there was an error parsing the commands.
            * @return true
            */
            bool doFail() override;
        };

        /**
        * Set the default error handle.
        * The default error handle is what's set as the error
        * handle of an ArgFilter if none are provided.
        * @note Stores a pointer to `handle`, so you must
        * keep it allocated.
        * @param [in] handle Reference to the new error handle
        * @see getDefaultErrorHandle()
        */
        static void setDefaultErrorHandle(ErrorHandle &handle);
        /**
        * Get the default error handle.
        * @return Reference to the current default error handle
        * @see setDefaultErrorHandle(ErrorHandle &handle)
        */
        static ErrorHandle &getDefaultErrorHandle();
        /**
        * Get the original default error handle.
        * @return The error handle
        * @see DefaultErrorHandle
        */
        static DefaultErrorHandle &getStoreDefaultErrorHandle();

    private:
        /// The original default error handle
        /// @see getStoreDefaultErrorHandle()
        static DefaultErrorHandle storeDefaultErrorHandle;
        /// The current default error handle
        /// @see setDefaultErrorHandle(ErrorHandle &handle)
        static ErrorHandle *defaultErrorHandle;

        /// All possible command parameters
        filters_t filters;
        /// Aliases to @ref filter "filters"
        aliases_t aliases;
        /// The max allowed flagless array parameters
        unsigned int maxArrayArgs;
        /// The command usage, printed by the shell
        /// if the command fails.
        std::string usage;
        /// The in-use error handle.
        ErrorHandle *errorHandle;
    public:
        /**
        * Construct a new ArgFilter with the default
        * error handle.
        * @see getDefaultErrorHandle()
        */
        ArgFilter();
        /**
        * Construct a new ArgFilter with a custom
        * error handle.
        * @see setErrorHandle(ErrorHandle &errorHandle)
        */
        ArgFilter(ErrorHandle &errorHandle);

        /**
        * Sets the error handle.
        * The error handle is used to process errors
        * when parsing command arguments.
        * @param [in] errorHandle The error handle
        * @see ArgFilter(ErrorHandle &errorHandle)
        * @see getErrorHandle()
        */
        void setErrorHandle(ErrorHandle &errorHandle);
        /**
        * Gets the error handle.
        * @return The error handle
        * @see setErrorHandle(ErrorHandle &errorHandle)
        */
        ErrorHandle &getErrorHandle();

        /**
        * Sets the maximum number of flagless parameters
        * that can be passed.
        * @param [in] count The max, inclusive
        */
        void setMaxArrayArgs(unsigned int count);

        /**
        * Sets the string that is printed when
        * the filter fails to parse arguments.
        * @param [in] usage The string to print
        * @see getUsage()
        */
        void setUsage(const std::string &usage);
        /**
        * Gets the command usage.
        * @return The usage.
        * @see setUsage(const std::string &usage)
        */
        std::string getUsage();

        /**
        * Add another filter, aka command parameter.
        * @note Does not account for filters that have the
        * same `name`.
        * @param [in] name The name of the parameter.
        * This is what's used to lookup the parameter.
        * @param [in] type The expected type of the parameter
        */
        void addFilter(const std::string &name, datatype type);
        // Adds alias for last filter entree.
        // Silently overwrites any other names.
        // Throws std::out_of_range if there are no filters
        /**
        * Adds alias for last filter entree.
        * @note Silently overwrites any pre-existing aliases
        * of the same name.
        * @param [in] name The alias
        * @throw std::out_of_range If there aren't any filters
        */
        void addAlias(const std::string &name);
        /**
        * Filter/process/parse given arguments.
        * @note Assumes that `arguments.size() > 0`
        * @param [in] arguments The arguments split up into parts
        * @param [out] out The Args object to store data in
        * @param [out] errMsg String to dump error messages
        * @return `true` if encountered an error
        */
        bool filter(const std::vector<std::string> &arguments, Args &out, std::string &errMsg);
    };
}

#endif
