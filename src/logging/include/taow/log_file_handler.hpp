#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
namespace TAOW::logging {

struct LogDirectoryManager {

    LogDirectoryManager(std::filesystem::path directory_path, int max_files, float max_file_size,
                        std::string file_prefix)
        : _directory_path{std::move(directory_path)}, _max_file_size{max_file_size}, _max_files{max_files},
          _file_prefix{std::move(file_prefix)} {}
    ~LogDirectoryManager() {
        if (_current_file_stream.is_open()) {
            _current_file_stream.close();
        }
    }

    void clean_directory() const;
    void write_to_last_file(std::string_view message);
    void update_current_file();

  private:
    const std::filesystem::path _directory_path;
    const int _max_files;
    const float _max_file_size;
    const std::string _file_prefix;
    std::ofstream _current_file_stream;

    void fetch_directory_log_files(std::vector<std::pair<int, const std::filesystem::path>>& result) const;
};

} // namespace TAOW::logging
