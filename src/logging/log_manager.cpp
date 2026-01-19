#include "taow/log_manager.hpp"
#include <iostream>
#include <mutex>
#include <sstream>
#include <utility>
#include <vector>

namespace TAOW::logging {

LogManager LogManager::instance = LogManager{};

void LogManager::push(LogLevel level, std::stringstream stream) {
    std::lock_guard<std::mutex> lock{this->_mutex};
    this->_log_queue.push_back({level, std::move(stream)});
    this->_cv.notify_one();
}

void LogManager::set_config(LogEnv env, LogConfig&& config) {
    std::lock_guard<std::mutex> lock{this->_mutex};
    this->_configs.try_emplace(env, std::move(config));
}

void LogManager::console_worker() {
    LogConfig config{};
    if (auto founded = this->_configs.find(LogEnv::CONSOLE); founded != this->_configs.end()) {
        config = founded->second;
    }
    std::unique_lock<std::mutex> lock{this->_mutex};
    while (true) {
        std::vector<std::pair<LogLevel, std::stringstream>> logs;
        bool should_break{false};
        {
            this->_cv.wait(lock, [this] { return this->_log_queue.size() > 0 || this->_is_finiesh; });
            logs = std::move(this->_log_queue);
            this->_log_queue = std::vector<std::pair<LogLevel, std::stringstream>>{};
            if (this->_is_finiesh) {
                should_break = true;
            }
        }
        for (auto& log : logs) {
            if (log.first < config.level)
                continue;
            std::cout << log.second.str();
        }
        if (should_break)
            break;
    }
}

} // namespace TAOW::logging
