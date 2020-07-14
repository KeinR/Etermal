#include "ArgFilter.h"

#include <memory>
#include <stdexcept>

#include "data/ArgData.h"
#include "data/data.h"

etm::ArgFilter::Filter::Filter(): type(datatype::STRING) {
}
etm::ArgFilter::Filter::Filter(const std::string &name, datatype type):
    name(name), type(type) {
}

etm::ArgFilter::ArgFilter() {
}
void etm::ArgFilter::setUsage(const std::string &usage) {
    this->usage = usage;
}
void etm::ArgFilter::addFilter(const std::string &name, datatype type) {
    filters.emplace_back(name, type);
    addAlias(name);
}
void etm::ArgFilter::addAlias(const std::string &name) {
    if (!filters.size()) {
        throw std::out_of_range("No filters exist to alias");
    }
    aliases[name] = filters.size() - 1;
}
std::string etm::ArgFilter::filter(const std::vector<std::string> &arguments, Args &out) {
    typedef filters_t::size_type size;

    out.setCall(arguments[0]);

    std::vector<bool> done(filters.size(), false);

    for (size i = 1; i < arguments.size(); i++) {
        aliases_t::iterator loc = aliases.find(arguments[i]);
        if (loc != aliases.end()) {
            Filter &f = filters[loc->second];
            if (f.type != datatype::BOOL) {
                if (i + 1 < arguments.size()) {
                    switch (f.type) {
                        case STRING: {
                            out.pushArg(filters[i].name, std::make_shared<data::String>(arguments[i + 1]));
                            break;
                        }
                        case INT: {
                            std::shared_ptr<data::Integer> data = std::make_shared<data::Integer>(arguments[i + 1]);
                            if (!data->failed()) {
                                out.pushArg(filters[i].name, data);
                            } else {
                                return "Expected \"" + arguments[i + 1] + "\" to be a int";
                            }
                            break;
                        }
                        case FLOAT: {
                            std::shared_ptr<data::Float> data = std::make_shared<data::Float>(arguments[i + 1]);
                            if (!data->failed()) {
                                out.pushArg(filters[i].name, data);
                            } else {
                                return "Expected \"" + arguments[i + 1] + "\" to be a float";
                            }
                            break;
                        }
                        default:
                            return "[Internal error]: invalid enum " + std::to_string(f.type);
                    }
                } else {
                    std::string tp;
                    switch (f.type) {
                        case STRING: tp = "string"; break;
                        case BOOL: tp = "bool"; break;
                        case INT: tp = "int"; break;
                        case FLOAT: tp = "float"; break;
                        default: tp = "[error]"; break;
                    }
                    return "Expected " + tp + " parameter after \"" + arguments[i] + "\"";
                }
            } else {
                done[loc->second] = true;
                out.pushArg(filters[i].name, std::make_shared<data::Boolean>(true));
            }
        } else {
            return "Unrecognized flag \"" + arguments[i] + "\"";
        }
    }

    // Add false flags, boolean flags that that weren't set.
    // Place for potentual optimization.
    for (size i = 0; i < filters.size(); i++) {
        if (filters[i].type == datatype::BOOL && !done[i]) {
            out.pushArg(filters[i].name, std::make_shared<data::Boolean>(false));
        }
    }

    return "";
}
