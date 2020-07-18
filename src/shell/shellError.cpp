#include "shellError.h"

etm::shellError::shellError(const std::string &location, const std::string &message, bool severe) noexcept:
    location(location), message(message), severe(severe) {
}
const char *etm::shellError::what() const noexcept {
    return message.c_str();
}
