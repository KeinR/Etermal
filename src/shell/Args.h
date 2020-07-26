#ifndef ETERMAL_ARGS_H_INCLUDED
#define ETERMAL_ARGS_H_INCLUDED

#include <memory>
#include <map>
#include <string>
#include <vector>

#include "data/ArgData.h"

namespace etm {

    /**
    * Data class containing information about arguments
    * passed to a command.
    * @see ArgFilter
    * @see Shell
    */
    class Args {
        /// Arg values
        /// @see data
        typedef std::shared_ptr<ArgData> data_t;
    private:
        /// Containter type for arguments
        typedef std::map<std::string, data_t> container_t;
        /// Arguments container
        container_t args;
        // Flagless parameters
        std::vector<std::string> arrayArgs;
        /// The alias that was used to invoke the command
        std::string call;
    public:
        /**
        * Construct an Args object.
        */
        Args();
        /** @internal
        * Adds a new argument.
        * @note For building only by @ref ArgFilter
        * @param [in] name The argument name
        * @param [in] data The argument data
        * @see has(const std::string &name)
        * @see get(const std::string &name)
        * @see args
        */
        void pushArg(const std::string &name, const data_t &data);
        /** @internal
        * Sets the alias that was used to invoke the command.
        * @param [in] str The alias
        * @see getCall()
        * @see call
        */
        void setCall(const std::string &str);
        /** @internal
        * Set the array args.
        * @param [in,out] The args (moved)
        */
        void setArrayArgs(std::vector<std::string> &&args);

        /**
        * Check if object @e has the given argument.
        * @param [in] name Name of the argument
        * @return `true` if the argument exists
        */
        bool has(const std::string &name);
        /**
        * Get the given argument.
        * @note If the argument doesn't exist
        * (@ref has(const std::string &name) returns false),
        * the returned shared pointer will not be callable.
        * @param [in] name Name of the argument
        */
        data_t get(const std::string &name);
        /**
        * Gets the alias that was used to invoke the command.
        * @return The alias
        */
        std::string &getCall();
        /**
        * Gets the flagless parameters
        * @return The args
        */
        std::vector<std::string> &getArrayArgs();
    };
}

#endif
