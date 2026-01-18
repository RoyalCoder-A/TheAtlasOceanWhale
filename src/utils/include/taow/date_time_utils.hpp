#pragma once

#include <chrono>
#include <string>
namespace TAOW::utils {

std::string time_point_to_string(const std::chrono::system_clock::time_point& tp, std::string format);
std::chrono::system_clock::time_point string_to_time_point(std::string date_time, std::string format);

} // namespace TAOW::utils
