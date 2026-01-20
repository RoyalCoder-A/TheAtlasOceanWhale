#include "taow/log_manager.hpp"
#include <iostream>
#include <mutex>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace TAOW::logging {

std::optional<LogManager> LogManager::instance;
void LogManager::instantiate(std::optional<ConsoleLogConfig> console_config, std::optional<FileLogConfig> file_config) {
    if (LogManager::instance) {
        throw std::runtime_error("Can't re-instantiate logger!");
    }
    LogManager::instance.emplace(ConstructorKey{}, console_config, file_config);
}
void LogManager::console_worker() {
    if (!_console_config) {
        throw std::runtime_error("Panic! console config not set but its worker has been triggered!");
    }
    auto config = _console_config.value();
    std::unique_lock<std::mutex> lock{this->_mutex};
    while (true) {
        std::vector<std::pair<LogLevel, std::stringstream>> logs;
        bool should_break{false};
        {
            this->_cv.wait(lock, [this] { return this->_log_queue.size() > 0 || this->_is_finiesh; });
            logs.swap(_log_queue);
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
