#pragma once

#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
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
                  std::optional<std::string> file_prefix = std::nullopt, float max_file_size_in_mb = 5,
                  int max_files_count = 5)
        : log_directory{log_directory}, level{level}, file_prefix{file_prefix},
          max_file_size_in_mb{max_file_size_in_mb}, max_files_count{max_files_count} {}

    LogLevel level;
    std::filesystem::path log_directory;
    std::optional<std::string> file_prefix;
    float max_file_size_in_mb;
    int max_files_count;
};

struct LogRecord {
    friend struct LogManager;
    struct ConstructorKey {
        friend struct LogManager;

      private:
        explicit ConstructorKey() {}
    };
    explicit LogRecord(LogRecord::ConstructorKey, LogLevel level, std::string_view class_name,
                       std::chrono::system_clock::time_point date_time, std::string message)
        : _level{level}, _class_name{class_name}, _date_time{std::move(date_time)}, _message{std::move(message)} {}

    LogRecord(const LogRecord& obj) = delete;
    LogRecord(LogRecord&& obj) = default;
    ~LogRecord() = default;

  private:
    LogLevel _level;
    std::string_view _class_name;
    std::chrono::system_clock::time_point _date_time;
    std::string _message;
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
            std::scoped_lock multi_lock{_console_mutex, _file_mutex};
            _is_finiesh = true;
            _console_cv.notify_all();
            _file_cv.notify_all();
        }
        for (auto& worker : this->_worker_threads) {
            worker.join();
        }
    }

    void push(LogLevel level, std::chrono::system_clock::time_point date_time, std::string_view class_name,
              std::string message);

  private:
    std::optional<ConsoleLogConfig> _console_config;
    std::optional<FileLogConfig> _file_config;
    std::vector<std::shared_ptr<LogRecord>> _console_log_queue;
    std::vector<std::shared_ptr<LogRecord>> _file_log_queue;
    std::mutex _console_mutex;
    std::condition_variable _console_cv;
    std::mutex _file_mutex;
    std::condition_variable _file_cv;
    std::vector<std::thread> _worker_threads;
    bool _is_finiesh{false};

    void console_worker();
    void file_worker();
};

} // namespace TAOW::logging
