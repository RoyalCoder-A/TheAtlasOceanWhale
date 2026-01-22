#include "taow/log_manager.hpp"
#include "taow/logging.hpp"
#include <filesystem>
struct Test {
    Test() {
        TAOW::logging::LogManager::instantiate(TAOW::logging::ConsoleLogConfig{},
                                               TAOW::logging::FileLogConfig{std::filesystem::current_path() / "logs"});
        TAOW::logging::Logger<Test>::info("Meow!");
        TAOW::logging::Logger<Test>::deubg("Another meow!");
        TAOW::logging::Logger<Test>::error("And another!");
    }
};

int main() {
    Test _{};
    return 0;
}
