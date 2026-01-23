#include "taow/log_file_handler.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <ios>
#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace TAOW::logging {

void LogDirectoryManager::clean_directory() const {
    std::vector<std::pair<int, const std::filesystem::path>> log_files{};
    fetch_directory_log_files(log_files);
    if (log_files.size() <= _max_files)
        return;

    log_files.resize(log_files.size() - _max_files);
    for (const auto& file : log_files) {
        std::filesystem::remove(file.second);
    }
}

void LogDirectoryManager::write_to_last_file(std::string_view message) { _current_file_stream << message; }

void LogDirectoryManager::update_current_file() {
    std::vector<std::pair<int, const std::filesystem::path>> log_files{};
    fetch_directory_log_files(log_files);
    std::optional<std::filesystem::path> new_file_path;
    if (log_files.size() == 0) {
        new_file_path = _directory_path / (_file_prefix + "-1.log");
    } else if (std::filesystem::file_size(log_files.end()->second) / (1024.0 * 1024.0) > _max_file_size) {
        new_file_path = _directory_path / (_file_prefix + "-" + std::to_string(log_files.end()->first + 1) + ".log");
    }
    if (!new_file_path)
        return;
    if (_current_file_stream.is_open())
        _current_file_stream.close();
    _current_file_stream = std::ofstream{new_file_path.value(), std::ios::app};
}

void LogDirectoryManager::fetch_directory_log_files(
    std::vector<std::pair<int, const std::filesystem::path>>& result) const {
    std::regex log_file_reg{R"(.*-(\d+)\.log)"};
    std::smatch match;
    for (const auto& file : std::filesystem::directory_iterator{_directory_path}) {
        auto file_name = file.path().filename().string();
        if (std::regex_match(file_name, match, log_file_reg)) {
            auto log_no = std::stoi(match[1].str());
            result.push_back({log_no, file});
        }
    }
    std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) { return a->first < b.first; });
}

} // namespace TAOW::logging
