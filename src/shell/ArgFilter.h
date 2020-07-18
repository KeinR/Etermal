#ifndef ARGFILTER_H_INCLUDED
#define ARGFILTER_H_INCLUDED

#include <string>
#include <vector>

#include "Args.h"

namespace etm {
    class ArgFilter {
    public:

        enum datatype {
            STRING,
            INT,
            FLOAT,
            BOOL
        };

        static const char *datatypeToString(datatype val);

        struct Filter {
            std::string name;
            datatype type;
            Filter();
            Filter(const std::string &name, datatype type);
        };

        typedef std::vector<Filter> filters_t;
        typedef std::map<std::string, std::vector<Filter>::size_type> aliases_t;

        class ErrorHandle {
        public:
            virtual ~ErrorHandle() = 0;

            // `position` is the argument index at which the error was
            // detected, including parameters.
            // The strings returned by all the functions are the
            // messages displayed to the user.
            // They can be empty if that's what you want.

            // The data cannot be parsed to the expected datatype
            virtual std::string badDatatype(int position, const std::string &data, datatype expectedType) = 0;
            // The flag was configured to accept a parameter, but instead
            // found the end of the commands list
            virtual std::string noParam(int position, const std::string &lastFlag, datatype expectedType) = 0;
            // The given flag does not exist
            virtual std::string badFlag(int position, const std::string &badFlag) = 0;
            // Some internal error happened
            virtual std::string internalError(int position, const std::string &errMsg) = 0;
            // Should the function exit as soon as
            // an error is detected, or should it let them pile up?
            virtual bool doFailfast() = 0;
            // If there was an error in the param parsing, should the
            // command still be executed?
            virtual bool doFail() = 0;
        };

        class DefaultErrorHandle: public ErrorHandle {
        public:
            DefaultErrorHandle();
            std::string badDatatype(int position, const std::string &data, datatype expectedType);
            std::string noParam(int position, const std::string &lastFlag, datatype expectedType);
            std::string badFlag(int position, const std::string &badFlag);
            std::string internalError(int position, const std::string &errMsg);
            bool doFailfast();
            bool doFail();
        };

        // Get/set the default error handles
        static void setDefaultErrorHandle(ErrorHandle &handle);
        static ErrorHandle &getDefaultErrorHandle();
        // The store default handle is the original default error
        // handle
        static DefaultErrorHandle &getStoreDefaultErrorHandle();

    private:
        static DefaultErrorHandle storeDefaultErrorHandle;
        static ErrorHandle *defaultErrorHandle;

        filters_t filters;
        aliases_t aliases;
        std::string usage;
        ErrorHandle *errorHandle;
    public:
        ArgFilter();
        // Following two constr/functions
        // set the handle for dealing with errors
        ArgFilter(ErrorHandle &errorHandle);

        void setErrorHandle(ErrorHandle &errorHandle);
        ErrorHandle &getErrorHandle();

        void setUsage(const std::string &usage);
        std::string getUsage();

        void addFilter(const std::string &name, datatype type);
        // Adds alias for last filter entree.
        // Silently overwrites any other names.
        // Throws std::out_of_range if there are no filters
        void addAlias(const std::string &name);
        // arguments including the invocation command.
        // Assumes that `arguments.size()` >= 1
        // Returns a non-empty string if given invalid args, ex. unexpected flags,
        // bad data types...
        bool filter(const std::vector<std::string> &arguments, Args &out, std::string &errMsg);
    };
}

#endif
