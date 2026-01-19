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
    static void info(const std::stringstream& stream) {
        Logger::log(LogLevel::INFO, stream, Modifier(ColorCode::BG_DEFAULT), Modifier(ColorCode::FG_GREEN));
    }
    static void error(const std::stringstream& stream) {
        Logger::log(LogLevel::ERROR, stream, Modifier(ColorCode::BG_DEFAULT), Modifier(ColorCode::FG_RED));
    }
    static void debug(const std::stringstream& stream) {
        Logger::log(LogLevel::DEBUG, stream, Modifier(ColorCode::BG_DEFAULT), Modifier(ColorCode::FG_BLUE));
    }

  private:
    static constexpr std::string_view _class_name = TAOW::utils::get_type_name<T>();
    static void log(LogLevel level, const std::stringstream& stream, const Modifier& fg, const Modifier& bg) {
        std::string prefix;
        switch (level) {
        case LogLevel::INFO:
            prefix = "INFO ";
            break;
        case DEBUG:
            prefix = "DEBUG";
            break;
        case ERROR:
            prefix = "ERROR";
            break;
            break;
        }
        std::stringstream os;
        os << fg << bg;
        os << prefix << " | ";
        os << _class_name << " | " << get_now() << " | ";
        os << stream.rdbuf();
        os << "\n";
        LogManager::instance.push(level, std::move(os));
    }
    static std::string get_now() {
        auto now = std::chrono::system_clock::now();
        return TAOW::utils::time_point_to_string(now, "%Y-%m-%d %H:%M:%S");
    }
};

} // namespace TAOW::logging
