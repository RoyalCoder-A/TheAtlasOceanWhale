#pragma once

#include <filesystem>
#include <string>

namespace TAOW::logger {
    enum LogLevel { DEBUG, INFO, WARNING, ERROR };

    struct BaseConfig {

        explicit BaseConfig(LogLevel level, const std::string& format,
                            bool is_json)
            : level{level}, log_format{format}, is_json{is_json} {};
        ~BaseConfig() = default;

        LogLevel level;
        std::string log_format;
        bool is_json;
    };

    struct ConsoleConfig : BaseConfig {
        explicit ConsoleConfig(LogLevel level, const std::string& format,
                               bool is_json)
            : BaseConfig(level, format, is_json) {};
        static ConsoleConfig from_yaml(const std::string& yml);
    };

    struct FileConfig : BaseConfig {

        explicit FileConfig(LogLevel level, const std::string& format,
                            bool is_json, unsigned short each_file_retention,
                            unsigned short old_files_retention_days,
                            std::filesystem::path log_directory,
                            std::string& files_prefix)
            : BaseConfig(level, format, is_json),
              each_file_retention_hours{each_file_retention},
              old_files_retention_days{old_files_retention_days},
              logs_directory{log_directory}, files_perfix{files_prefix} {};

        static FileConfig from_yaml(const std::string& yml);
        unsigned short each_file_retention_hours;
        unsigned short old_files_retention_days;
        std::filesystem::path logs_directory;
        std::string files_perfix;
    };
} // namespace TAOW::logger