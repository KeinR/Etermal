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
            BOOL,
            INT,
            FLOAT
        };

        struct Filter {
            std::string name;
            datatype type;
            Filter();
            Filter(const std::string &name, datatype type);
        };

        typedef std::vector<Filter> filters_t;
        typedef std::map<std::string, std::vector<Filter>::size_type> aliases_t;

    private:
        filters_t filters;
        aliases_t aliases;
        std::string usage;
    public:
        ArgFilter();
        void setUsage(const std::string &usage);
        void addFilter(const std::string &name, datatype type);
        // Adds alias for last filter entree.
        // Silently overwrites any other names.
        // Throws std::out_of_range if there are no filters
        void addAlias(const std::string &name);
        // arguments including the invocation command.
        // Assumes that `arguments.size()` >= 1
        // Returns a non-empty string if given invalid args, ex. unexpected flags,
        // bad data types...
        std::string filter(const std::vector<std::string> &arguments, Args &out);
    };
}

#endif
