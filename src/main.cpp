#include "taow/logging.hpp"
#include <chrono>
#include <sstream>
#include <thread>
struct Test {
    Test() {
        std::stringstream log;
        log << "Meow!";
        TAOW::logging::Logger<Test>::info(log);
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::stringstream new_log;
        new_log << "Meow Meow!";
        TAOW::logging::Logger<Test>::debug(new_log);
    }
};

int main() {
    Test _{};
    return 0;
}
