#include "taow/log_manager.hpp"
#include "taow/logging.hpp"
#include <chrono>
#include <filesystem>
#include <thread>
struct Test {
    Test() {
        TAOW::logging::LogManager::instantiate(TAOW::logging::ConsoleLogConfig{TAOW::logging::LogLevel::INFO},
                                               TAOW::logging::FileLogConfig{std::filesystem::current_path() / "logs",
                                                                            TAOW::logging::LogLevel::DEBUG, "taow", 0.1,
                                                                            2});
        for (int i = 0; i < 100000; ++i) {
            TAOW::logging::Logger<Test>::info(std::to_string(i) + "- Meow!");
            TAOW::logging::Logger<Test>::deubg("Another meow!");
            TAOW::logging::Logger<Test>::error("And another!");
        }
    }
};

int main() {
    Test _{};
    return 0;
}
