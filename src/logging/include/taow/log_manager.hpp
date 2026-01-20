#pragma once

#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>
namespace TAOW::logging {

enum LogLevel { DEBUG = 0, INFO = 1, ERROR = 2 };
enum LogEnv { CONSOLE = 0, FILE = 1 };

struct ConsoleLogConfig {
    ConsoleLogConfig(LogLevel level = LogLevel::INFO) : level{level} {}
    LogLevel level;
};

struct FileLogConfig {

    FileLogConfig(std::filesystem::path log_directory, LogLevel level = LogLevel::INFO,
                  std::optional<std::string> file_prefix = std::nullopt)
        : log_directory{log_directory}, level{level}, file_prefix{file_prefix} {}

    LogLevel level;
    std::filesystem::path log_directory;
    std::optional<std::string> file_prefix;
};

struct LogManager {
    struct ConstructorKey {
        friend struct LogManager;

      private:
        explicit ConstructorKey() = default;
    };
    LogManager(ConstructorKey, std::optional<ConsoleLogConfig> console_config,
               std::optional<FileLogConfig> file_config) {
        if (!console_config && !file_config) {
            throw std::runtime_error("Can't instantiate logger without any configs!");
        }
        if (console_config) {
            _console_config = console_config;
            _worker_threads.emplace_back(&LogManager::console_worker, this);
        }
        if (file_config) {
            _file_config = file_config;
            _worker_threads.emplace_back(&LogManager::file_worker, this);
        }
    }

    static std::optional<LogManager> instance;
    static void instantiate(std::optional<ConsoleLogConfig> console_config, std::optional<FileLogConfig> file_config);

    ~LogManager() {
        {
            std::lock_guard<std::mutex> lock{_mutex};
            _is_finiesh = true;
            this->_cv.notify_all();
        }
        for (auto& worker : this->_worker_threads) {
            worker.join();
        }
    }

    void push(LogLevel level, std::string_view class_name, std::chrono::system_clock::time_point date_time,
              std::stringstream stream);

  private:
    std::optional<ConsoleLogConfig> _console_config;
    std::optional<FileLogConfig> _file_config;
    std::vector<std::pair<LogLevel, std::stringstream>> _log_queue;
    std::mutex _mutex;
    std::condition_variable _cv;
    std::vector<std::thread> _worker_threads;
    bool _is_finiesh{false};

    void console_worker();
    void file_worker();
};

} // namespace TAOW::logging
