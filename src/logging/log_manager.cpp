#include "taow/log_manager.hpp"
#include "taow/date_time_utils.hpp"
#include "taow/log_file_handler.hpp";
#include "taow/logging.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

namespace TAOW::logging {

std::string get_color_code(ColorCode code) { return "\033[" + std::to_string(code) + "m"; }

std::optional<LogManager> LogManager::instance;

void LogManager::instantiate(std::optional<ConsoleLogConfig> console_config, std::optional<FileLogConfig> file_config) {
    if (LogManager::instance) {
        throw std::runtime_error("Can't re-instantiate logger!");
    }
    LogManager::instance.emplace(ConstructorKey{}, console_config, file_config);
}

void LogManager::push(LogLevel level, std::chrono::system_clock::time_point date_time, std::string_view class_name,
                      std::string message) {
    auto record = std::make_shared<LogRecord>(LogRecord::ConstructorKey{}, level, class_name, std::move(date_time),
                                              std::move(message));
    if (_console_config) {
        std::lock_guard<std::mutex> lock{_console_mutex};
        _console_log_queue.push_back(record);
        _console_cv.notify_one();
    }
    if (_file_config) {
        std::lock_guard<std::mutex> lock{_file_mutex};
        _file_log_queue.push_back(record);
        _file_cv.notify_one();
    }
}

void LogManager::console_worker() {
    if (!_console_config) {
        throw std::runtime_error("Panic! console config not set but its worker has been triggered!");
    }
    auto config = _console_config.value();
    bool should_break{false};
    while (true) {
        std::vector<std::shared_ptr<LogRecord>> logs;
        {
            std::unique_lock lock{_console_mutex};
            _console_cv.wait(lock, [this] { return _console_log_queue.size() > 0 || _is_finiesh; });
            logs.swap(_console_log_queue);
            if (_is_finiesh) {
                should_break = true;
            }
        }
        for (auto& log : logs) {
            if (log->_level < config.level)
                continue;
            ColorCode code;
            std::string_view prefix;
            switch (log->_level) {
            case LogLevel::INFO:
                code = ColorCode::FG_GREEN;
                prefix = "INFO";
                break;
            case LogLevel::DEBUG:
                code = ColorCode::FG_BLUE;
                prefix = "DEBUG";
                break;
            case LogLevel::ERROR:
                code = ColorCode::FG_RED;
                prefix = "ERROR";
                break;
            }
            std::cout << get_color_code(ColorCode::BG_DEFAULT) << get_color_code(code) << "[" << prefix << "] "
                      << log->_class_name << " "
                      << TAOW::utils::time_point_to_string(log->_date_time, "%Y-%m-%d %H:%M:%S") << " " << log->_message
                      << "\n";
        }
        std::this_thread::sleep_for(std::chrono::seconds{2});
        if (should_break)
            break;
    }
}

void LogManager::file_worker() {
    if (!_file_config) {
        throw std::runtime_error("Panic! no file configuration provided but worker triggered!");
    }
    auto config = _file_config.value();
    LogDirectoryManager file_handler{config.log_directory, config.max_files_count, config.max_file_size_in_mb,
                                     config.file_prefix ? config.file_prefix.value() : ""};
    bool should_break{false};
    while (true) {
        std::vector<std::shared_ptr<LogRecord>> logs;
        {
            std::unique_lock lock{_file_mutex};
            _file_cv.wait(lock, [this] { return this->_file_log_queue.size() > 0 || this->_is_finiesh; });
            logs.swap(this->_file_log_queue);
            if (_is_finiesh) {
                should_break = true;
            }
        }
        file_handler.clean_directory();
        file_handler.update_current_file();
        for (auto& log : logs) {
            std::string_view prefix;
            switch (log->_level) {
            case LogLevel::INFO:
                prefix = "INFO";
                break;
            case LogLevel::ERROR:
                prefix = "ERROR";
                break;
            case LogLevel::DEBUG:
                prefix = "DEBUG";
                break;
            }
            std::stringstream ss;
            ss << "[" << prefix << "] " << log->_class_name << " "
               << TAOW::utils::time_point_to_string(log->_date_time, "%Y-%m-%d %H:%M:%S") << " " << log->_message
               << "\n";
            file_handler.write_to_last_file(ss.str());
        }
        if (should_break)
            break;
    }
}

} // namespace TAOW::logging
