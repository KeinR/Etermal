#include "error.h"

etm::be_error::be_error(const std::string &message) noexcept:
    message(message) {
}
const char *etm::be_error::what() const noexcept {
    return message.c_str();
}
