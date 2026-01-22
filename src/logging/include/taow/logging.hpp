#pragma once

#include "taow/log_manager.hpp"
#include "taow/type_utils.hpp"
#include <chrono>
#include <cstdio>
#include <ctime>
#include <stdexcept>
#include <string>
#include <string_view>
namespace TAOW::logging {

enum ColorCode {
    FG_RED = 31,
    FG_GREEN = 32,
    FG_BLUE = 34,
    FG_DEFAULT = 39,
    BG_RED = 41,
    BG_GREEN = 42,
    BG_BLUE = 44,
    BG_DEFAULT = 49
};

template <typename T> struct Logger {
    static void info(std::string message) { Logger<T>::log(LogLevel::INFO, std::move(message)); }
    static void deubg(std::string message) { Logger<T>::log(LogLevel::DEBUG, std::move(message)); }
    static void error(std::string message) { Logger<T>::log(LogLevel::ERROR, std::move(message)); }

  private:
    static constexpr std::string_view _class_name = TAOW::utils::get_type_name<T>();
    static void log(LogLevel level, std::string message) {
        if (!LogManager::instance) {
            throw std::runtime_error("Log manager has not been instantiated!");
        }
        LogManager::instance->push(level, std::chrono::system_clock::now(), _class_name, std::move(message));
    }
};

} // namespace TAOW::logging
