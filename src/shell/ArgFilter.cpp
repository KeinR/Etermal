#include "ArgFilter.h"

#include <memory>
#include <stdexcept>
#include <utility>

#include "data/ArgData.h"
#include "data/data.h"

etm::ArgFilter::ErrorHandle::~ErrorHandle() {
}

etm::ArgFilter::DefaultErrorHandle etm::ArgFilter::storeDefaultErrorHandle;
etm::ArgFilter::ErrorHandle *etm::ArgFilter::defaultErrorHandle = &storeDefaultErrorHandle;

void etm::ArgFilter::setDefaultErrorHandle(ErrorHandle &handle) {
    defaultErrorHandle = &handle;
}
etm::ArgFilter::ErrorHandle &etm::ArgFilter::getDefaultErrorHandle() {
    return *defaultErrorHandle;
}

etm::ArgFilter::DefaultErrorHandle &etm::ArgFilter::getStoreDefaultErrorHandle() {
    return storeDefaultErrorHandle;
}

const char *etm::ArgFilter::datatypeToString(datatype val) {
    switch (val) {
        case STRING: return "string";
        case INT: return "int";
        case FLOAT: return "float";
        case BOOL: return "bool";
        default: return "[invalid enum]";
    }
}

etm::ArgFilter::DefaultErrorHandle::DefaultErrorHandle() {
}
std::string etm::ArgFilter::DefaultErrorHandle::badDatatype(int position, const std::string &data, datatype expectedType) {
    return "Expected \"" + data + "\" to be of " + datatypeToString(expectedType) + " type @" + std::to_string(position) + "\n";
}
std::string etm::ArgFilter::DefaultErrorHandle::noParam(int position, const std::string &lastFlag, datatype expectedType) {
    return "Expected parameter after \"" + lastFlag + "\" @" + std::to_string(position) + "\n";
}
std::string etm::ArgFilter::DefaultErrorHandle::badFlag(int position, const std::string &badFlag) {
    return "Unknown flag \"" + badFlag + "\" @" + std::to_string(position) + "\n";
}
std::string etm::ArgFilter::DefaultErrorHandle::tooManyArgs(int position, unsigned int max) {
    return "Too many args given, max = " + std::to_string(max) + " @" + std::to_string(position) + "\n";
}
std::string etm::ArgFilter::DefaultErrorHandle::internalError(int position, const std::string &errMsg) {
    return "Internal error \"" + errMsg + "\" @" + std::to_string(position) + "\n";
}
bool etm::ArgFilter::DefaultErrorHandle::doFailfast() {
    return true;
}
bool etm::ArgFilter::DefaultErrorHandle::doFail() {
    return true;
}

etm::ArgFilter::Filter::Filter(): type(datatype::STRING) {
}
etm::ArgFilter::Filter::Filter(const std::string &name, datatype type):
    name(name), type(type) {
}

etm::ArgFilter::ArgFilter(): ArgFilter(getDefaultErrorHandle()) {
}
etm::ArgFilter::ArgFilter(ErrorHandle &errorHandle): maxArrayArgs(10) {
    setErrorHandle(errorHandle);
}

void etm::ArgFilter::setErrorHandle(ErrorHandle &handle) {
    errorHandle = &handle;
}
etm::ArgFilter::ErrorHandle &etm::ArgFilter::getErrorHandle() {
    return *errorHandle;
}

void etm::ArgFilter::setMaxArrayArgs(unsigned int count) {
    maxArrayArgs = count;
}

void etm::ArgFilter::setUsage(const std::string &usage) {
    this->usage = usage;
}
std::string etm::ArgFilter::getUsage() {
    return usage;
}

void etm::ArgFilter::addFilter(const std::string &name, datatype type) {
    filters.emplace_back(name, type);
    addAlias("-" + name);
}
void etm::ArgFilter::addAlias(const std::string &name) {
    if (!filters.size()) {
        throw std::out_of_range("No filters exist to alias");
    }
    aliases["-" + name] = filters.size() - 1;
}
bool etm::ArgFilter::filter(const std::vector<std::string> &arguments, Args &out, std::string &errMsg) {
    typedef filters_t::size_type size;

    // Was an error set?
    bool failed = false;

    // First argument is the invokation alias
    out.setCall(arguments[0]);

    // Used to keep track of which bool flags were set
    std::vector<bool> done(filters.size(), false);

    // Flagless parameters
    std::vector<std::string> arrayArgs;

    for (size i = 1; i < arguments.size(); i++) {
        bool loopError = false;
        if (arguments[i][0] == '-') {
            // Is flag
            aliases_t::iterator loc = aliases.find(arguments[i]);
            if (loc != aliases.end()) {
                Filter &f = filters[loc->second];
                if (f.type != datatype::BOOL) {
                    if (i + 1 < arguments.size()) {
                        std::shared_ptr<ArgData> data;
                        switch (f.type) {
                            case STRING: {
                                data = std::make_shared<data::String>(arguments[i + 1]);
                                break;
                            }
                            case INT: {
                                try {
                                    data = std::make_shared<data::Integer>(std::stoi(arguments[i + 1]));
                                } catch (std::exception &e) {
                                    loopError = true;
                                    errMsg += errorHandle->badDatatype(i + 1, arguments[i + 1], f.type);
                                }
                                break;
                            }
                            case FLOAT: {
                                try {
                                    std::make_shared<data::Float>(std::stof(arguments[i + 1]));
                                } catch (std::exception &e) {
                                    loopError = true;
                                    errMsg += errorHandle->badDatatype(i + 1, arguments[i + 1], f.type);
                                }
                                break;
                            }
                            default:
                                loopError = true;
                                errMsg += errorHandle->internalError(i + 1, "Invalid enum for datatype \"" + std::to_string(f.type) + "\"");
                        }

                        i++;

                        if (!loopError) {
                            out.pushArg(filters[i].name, data);
                        }

                    } else {
                        loopError = true;
                        errMsg += errorHandle->noParam(i + 1, arguments[i], f.type);
                    }
                } else {
                    // If it's of a bool type, the presence of the flag sets it to `true`
                    done[loc->second] = true;
                    out.pushArg(filters[i].name, std::make_shared<data::Boolean>(true));
                }
            } else {
                // The flag doesn't exist
                loopError = true;
                errMsg += errorHandle->badFlag(i, arguments[i]);
            }
        } else {
            // Is array param

            if (arrayArgs.size() < maxArrayArgs) {
                arrayArgs.push_back(arguments[i]);
            } else {
                // Too many args
                loopError = true;
                errMsg += errorHandle->tooManyArgs(i, maxArrayArgs);
            }
        }

        if (loopError) {
            // Only stop and exit now if failfast is true
            if (errorHandle->doFailfast()) {
                return true;
            }
            failed = true;
        }
    }

    out.setArrayArgs(std::move(arrayArgs));

    // Add false flags, boolean flags that that weren't set.
    // Place for potentual optimization.
    for (size i = 0; i < filters.size(); i++) {
        if (filters[i].type == datatype::BOOL && !done[i]) {
            out.pushArg(filters[i].name, std::make_shared<data::Boolean>(false));
        }
    }

    return failed;
}
