#include "termError.h"

etm::termError::termError(const std::string &location, const std::string &message, int code, bool severe) noexcept:
    location(location), message(message), code(code), severe(severe) {
}
const char *etm::termError::what() const noexcept {
    return message.c_str();
}
