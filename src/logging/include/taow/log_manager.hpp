#pragma once

#include <condition_variable>
#include <mutex>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>
namespace TAOW::logging {

enum LogLevel { DEBUG = 0, INFO = 1, ERROR = 2 };
enum LogEnv { CONSOLE = 0 };

struct LogConfig {
    LogConfig() : level(LogLevel::INFO) {};
    LogConfig(LogLevel level) : level(level) {}

    LogLevel level;
};

struct LogManager {
    static LogManager instance;
    void push(LogLevel level, std::stringstream stream);

    ~LogManager() {
        {
            std::lock_guard<std::mutex> lock{_mutex};
            _is_finiesh = true;
            this->_cv.notify_all();
        }
        this->_worker_thread.join();
    }

    void set_config(LogEnv env, LogConfig&& config);

  private:
    LogManager() { _worker_thread = std::thread(&LogManager::console_worker, this); }
    std::vector<std::pair<LogLevel, std::stringstream>> _log_queue;
    std::mutex _mutex;
    std::condition_variable _cv;
    std::thread _worker_thread;
    bool _is_finiesh{false};
    std::unordered_map<LogEnv, LogConfig> _configs;

    void console_worker();
};

} // namespace TAOW::logging
