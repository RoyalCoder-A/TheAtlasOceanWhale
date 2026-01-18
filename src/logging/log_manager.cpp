#include "taow/log_manager.hpp"
#include <iostream>
#include <mutex>
#include <sstream>
#include <utility>
#include <vector>

namespace TAOW::logging {

LogManager LogManager::instance = LogManager{};

void LogManager::push(std::stringstream stream) {
    std::lock_guard<std::mutex> lock{this->_mutex};
    this->_log_queue.push_back(std::move(stream));
    this->_cv.notify_one();
}

void LogManager::worker() {
    std::unique_lock<std::mutex> lock{this->_mutex};
    while (!this->_is_finiesh) {
        std::vector<std::stringstream> logs;
        {
            this->_cv.wait(lock, [this] { return this->_log_queue.size() > 0 || this->_is_finiesh; });
            logs = std::move(this->_log_queue);
            this->_log_queue = std::vector<std::stringstream>{};
        }
        for (auto& log : logs) {
            std::cout << log.str();
        }
    }
}

} // namespace TAOW::logging
