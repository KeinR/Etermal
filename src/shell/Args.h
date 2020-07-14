#ifndef ETERMAL_ARGS_H_INCLUDED
#define ETERMAL_ARGS_H_INCLUDED

#include <memory>
#include <map>
#include <string>

#include "data/ArgData.h"

namespace etm {
    class Args {
        typedef std::shared_ptr<ArgData> data_t;
    private:
        typedef std::map<std::string, data_t> container_t;
        container_t args;
        std::string call;
    public:
        Args();
        // Builders
        void pushArg(const std::string &name, const data_t &data);
        void setCall(const std::string &str);

        // Accessors
        bool has(const std::string &name);
        data_t get(const std::string &name);
        // The invokation command
        std::string &getCall();
    };
}

#endif
