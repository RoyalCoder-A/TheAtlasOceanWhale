#include "taow/date_time_utils.hpp"
#include "taow/utils_exception.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace TAOW::utils {

std::string time_point_to_string(const std::chrono::system_clock::time_point& tp, std::string format) {
    auto in_time_t = std::chrono::system_clock::to_time_t(tp);
    std::tm time_struct;
#if defined(_MSC_VER)
    localtime_s(&time_struct, &in_time_t);
#else
    localtime_r(&in_time_t, &time_struct);
#endif
    std::stringstream ss;
    ss << std::put_time(&time_struct, format.c_str());
    return ss.str();
}

std::chrono::system_clock::time_point string_to_time_point(std::string date_time, std::string format) {
    std::stringstream ss{date_time};
    std::tm time_struct;
    ss >> std::get_time(&time_struct, format.c_str());
    if (ss.fail()) {
        throw ParseDateError("Failed to parse date! date: " + date_time + "format: " + format);
    }
    auto in_time_t = std::mktime(&time_struct);
    return std::chrono::system_clock::from_time_t(in_time_t);
}

} // namespace TAOW::utils
