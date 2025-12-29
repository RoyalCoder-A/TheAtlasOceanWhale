#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace TAOW::http_client {

struct FileUpload {
    FileUpload(std::string name, std::string content_type, std::vector<std::uint8_t> bytes)
        : name(std::move(name)), content_type(std::move(content_type)), bytes(std::move(bytes)) {}

    const std::string name;
    const std::string content_type;
    const std::vector<std::uint8_t> bytes;
};

struct Multipart {
    Multipart(std::unordered_map<std::string, FileUpload> files, std::unordered_map<std::string, std::string> fields)
        : _files(std::move(files)), _fields(std::move(fields)) {}

    const std::vector<std::uint8_t> as_http_body(std::string_view boundary) const;

  private:
    const std::unordered_map<std::string, FileUpload> _files;
    const std::unordered_map<std::string, std::string> _fields;
};

} // namespace TAOW::http_client