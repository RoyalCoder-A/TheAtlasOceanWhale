#include "taow/cli_builder.hpp"
#include <cstddef>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace TAOW::cli_builder {

template <>
std::optional<std::string> get_option<std::string>(std::string_view option_name,
                                                   const std::unordered_map<std::string, std::string>& raw_options) {
    if (auto found = raw_options.find(std::string{option_name}); found != raw_options.end()) {
        return found->second;
    }
    return std::nullopt;
}

template <>
std::optional<int> get_option<int>(std::string_view option_name,
                                   const std::unordered_map<std::string, std::string>& raw_options) {
    if (auto found = raw_options.find(std::string{option_name}); found != raw_options.end()) {
        return std::stoi(found->second);
    }
    return std::nullopt;
}

template <>
std::optional<double> get_option<double>(std::string_view option_name,
                                         const std::unordered_map<std::string, std::string>& raw_options) {
    if (auto found = raw_options.find(std::string{option_name}); found != raw_options.end()) {
        return std::stod(found->second);
    }
    return std::nullopt;
}

template <>
std::optional<bool> get_option<bool>(std::string_view option_name,
                                     const std::unordered_map<std::string, std::string>& raw_options) {
    if (auto found = raw_options.find(std::string{option_name}); found != raw_options.end()) {
        return found->second == "1" | found->second == "t" | found->second == "y";
    }
    return std::nullopt;
}

template <> std::string get_arg<std::string>(std::size_t idx, const std::vector<std::string>& args) {
    if (idx >= args.size()) {
        throw std::runtime_error("Args not passed correctly! see --help");
    }
    return args[idx];
}

template <> int get_arg<int>(std::size_t idx, const std::vector<std::string>& args) {
    if (idx >= args.size()) {
        throw std::runtime_error("Args not passed correctly! see --help");
    }
    return std::stoi(args[idx]);
}

template <> double get_arg<double>(std::size_t idx, const std::vector<std::string>& args) {
    if (idx >= args.size()) {
        throw std::runtime_error("Args not passed correctly! see --help");
    }
    return std::stod(args[idx]);
}

template <> bool get_arg<bool>(std::size_t idx, const std::vector<std::string>& args) {
    if (idx >= args.size()) {
        throw std::runtime_error("Args not passed correctly! see --help");
    }
    return args[idx] == "1" || args[idx] == "t" || args[idx] == "y";
}

int CliRegistry::run(int argc, char** argv) {
    if (argc < 2) {
        throw std::runtime_error("No args passed!");
    }
    std::vector<std::string> raw_args{argv + 1, argv + argc};
    for (const auto& [k, v] : CliRegistry::commands) {
        std::cout << k << std::endl;
    }
    auto found_command = CliRegistry::commands.find(raw_args[0]);
    if (found_command == CliRegistry::commands.end()) {
        throw std::runtime_error("No command found for " + raw_args[0]);
    }
    std::vector<std::string> args;
    std::unordered_map<std::string, std::string> options;
    args.reserve(argc);
    options.reserve(argc);
    for (std::size_t i = 1; i < raw_args.size(); ++i) {
        if (raw_args[i].find("--") == 0) {
            auto full_option = raw_args[i].substr(2);
            auto key_val_sep = full_option.find("=");
            std::string key, val;
            if (key_val_sep == std::string::npos) {
                key = full_option;
                val = "1";
            } else {
                key = full_option.substr(0, key_val_sep);
                val = full_option.substr(key_val_sep + 1);
            }
            options[key] = val;
            continue;
        }
        args.push_back(raw_args[i]);
    }
    found_command->second.options->from_options(options);
    found_command->second.args->from_args(args);
    return found_command->second.callable(*found_command->second.options, *found_command->second.args);
}

} // namespace TAOW::cli_builder
