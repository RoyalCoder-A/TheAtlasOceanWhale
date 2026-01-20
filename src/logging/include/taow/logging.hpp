#pragma once

#include "taow/date_time_utils.hpp"
#include "taow/log_manager.hpp"
#include "taow/type_utils.hpp"
#include <chrono>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <ostream>
#include <sstream>
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

struct Modifier {

    Modifier(ColorCode code) : _color_code(code) {}

    friend std::stringstream& operator<<(std::stringstream& os, const Modifier& md);

  private:
    const ColorCode _color_code;
};

template <typename T> struct Logger {
    static void info(std::stringstream stream) {
        Logger::log(LogLevel::INFO, std::move(stream), Modifier(ColorCode::BG_DEFAULT), Modifier(ColorCode::FG_GREEN));
    }
    static void error(std::stringstream stream) {
        Logger::log(LogLevel::ERROR, std::move(stream), Modifier(ColorCode::BG_DEFAULT), Modifier(ColorCode::FG_RED));
    }
    static void debug(std::stringstream stream) {
        Logger::log(LogLevel::DEBUG, std::move(stream), Modifier(ColorCode::BG_DEFAULT), Modifier(ColorCode::FG_BLUE));
    }

  private:
    static constexpr std::string_view _class_name = TAOW::utils::get_type_name<T>();
    static void log(LogLevel level, std::stringstream stream, const Modifier& fg, const Modifier& bg) {
        if (!LogManager::instance) {
            throw std::runtime_error("Log manager has not been instantiated!");
        }
        LogManager::instance->push(level, _class_name, std::chrono::system_clock::now(), std::move(stream));
    }
};

} // namespace TAOW::logging
