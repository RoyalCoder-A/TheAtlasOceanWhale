#pragma once

#include <stdexcept>
#include <string>
namespace TAOW::cli_builder {
struct CLIException : std::runtime_error {
    CLIException(std::string message) : std::runtime_error(std::move(message)) {}
};

struct CommandNotFound : CLIException {
    CommandNotFound(std::string message) : CLIException(std::move(message)) {}
};

struct ArgsNotCorrect : CLIException {
    ArgsNotCorrect(std::string message) : CLIException(std::move(message)) {}
};
} // namespace TAOW::cli_builder
