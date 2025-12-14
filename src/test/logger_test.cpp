#include "catch2/catch_test_macros.hpp"
#include "logger_config.hpp"

TEST_CASE("Logger tests") {
    SECTION("Config tests") {
        const auto console_config = TAOW::logger::ConsoleConfig::from_yaml(R"(
            is_json: false
            level: INFO
            format: "[{}] {} => {}; {}"
            )");
        REQUIRE_FALSE(console_config.is_json);
        REQUIRE(console_config.level == TAOW::logger::LogLevel::INFO);
    }
}