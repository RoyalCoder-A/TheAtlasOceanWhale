#include "taow/logging.hpp"
#include <sstream>

namespace TAOW::logging {
std::stringstream& operator<<(std::stringstream& os, const Modifier& md) {
    os << "\033[" << md._color_code << "m";
    return os;
}
} // namespace TAOW::logging
