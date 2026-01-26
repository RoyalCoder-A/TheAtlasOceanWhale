#pragma once

#include <stdexcept>
#include <string>
#include <utility>
namespace TAOW::logging {

struct LoggingException : std::runtime_error {
    LoggingException(std::string message) : std::runtime_error(std::move(message)) {}
};

struct LogConfigNotFound : LoggingException {
    LogConfigNotFound(std::string message) : LoggingException(std::move(message)) {}
};

struct LogReinitializerError : LoggingException {
    LogReinitializerError(std::string message) : LoggingException(std::move(message)) {}
};

struct EmptyInitializerError : LoggingException {
    EmptyInitializerError(std::string message) : LoggingException(std::move(message)) {}
};

struct LogManagerNotInstantiatedError : LoggingException {
    LogManagerNotInstantiatedError(std::string message) : LoggingException(std::move(message)) {}
};

} // namespace TAOW::logging
