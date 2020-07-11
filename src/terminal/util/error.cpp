#include "error.h"

etm::fe_error::fe_error(const std::string &message) noexcept:
    message(message) {
}
const char *etm::fe_error::what() const noexcept {
    return message.c_str();
}
