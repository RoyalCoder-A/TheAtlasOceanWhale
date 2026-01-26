#pragma once

#include <stdexcept>
#include <string>
#include <utility>
namespace TAOW::utils {

struct UtilsError : std::runtime_error {
    UtilsError(std::string message) : std::runtime_error(std::move(message)) {}
};

struct ParseDateError : UtilsError {
    ParseDateError(std::string message) : UtilsError(std::move(message)) {}
};

} // namespace TAOW::utils
