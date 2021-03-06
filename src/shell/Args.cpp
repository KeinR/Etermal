#include "Args.h"

etm::Args::Args() {
}
void etm::Args::pushArg(const std::string &name, const data_t &data) {
    args[name] = data;
}
void etm::Args::setCall(const std::string &str) {
    call = str;
}
void etm::Args::setArrayArgs(std::vector<std::string> &&args) {
    arrayArgs = std::move(args);
}

bool etm::Args::has(const std::string &name) {
    return args.find(name) != args.end();
}
etm::Args::data_t etm::Args::get(const std::string &name) {
    return args[name];
}
std::string &etm::Args::getCall() {
    return call;
}
std::vector<std::string> &etm::Args::getArrayArgs() {
    return arrayArgs;
}
