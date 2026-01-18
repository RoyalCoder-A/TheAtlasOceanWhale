#pragma once

#include <condition_variable>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>
namespace TAOW::logging {

struct LogManager {
    static LogManager instance;
    void push(std::stringstream stream);

    ~LogManager() {
        {
            std::lock_guard<std::mutex> lock{_mutex};
            _is_finiesh = true;
            this->_cv.notify_all();
        }
        this->_worker_thread.join();
    }

  private:
    LogManager() { _worker_thread = std::thread(&LogManager::worker, this); }
    std::vector<std::stringstream> _log_queue;
    std::mutex _mutex;
    std::condition_variable _cv;
    std::thread _worker_thread;
    bool _is_finiesh{false};

    void worker();
};

} // namespace TAOW::logging
