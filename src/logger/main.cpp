#include "logger_config.hpp"
#include <string>
#include <yaml-cpp/yaml.h>

namespace TAOW::logger {
    ConsoleConfig ConsoleConfig::from_yaml(const std::string& yml) {
        const YAML::Node node = YAML::Load(yml);
        return ConsoleConfig{node["level"].as<LogLevel>(),
                             node["format"].as<std::string>(),
                             node["is_json"].as<bool>()};
    }
} // namespace TAOW::logger

namespace YAML {
    template <> struct convert<TAOW::logger::LogLevel> {
        static bool decode(const Node& node, TAOW::logger::LogLevel& rhs) {
            auto s = node.as<std::string>();
            if (s == "DEBUG")
                rhs = TAOW::logger::LogLevel::DEBUG;
            else if (s == "INFO")
                rhs = TAOW::logger::LogLevel::INFO;
            else if (s == "WARN")
                rhs = TAOW::logger::LogLevel::WARNING;
            else if (s == "ERROR")
                rhs = TAOW::logger::LogLevel::ERROR;
            else
                return false;
            return true;
        }
    };
} // namespace YAML