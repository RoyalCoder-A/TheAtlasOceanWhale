#include "taow/http_client.hpp"
#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <unordered_map>
#include <utility>
#include <vector>

namespace TAOW::http_client {

std::vector<std::uint8_t> process_chunked_response(const std::vector<uint8_t>& body_bytes) {
    const std::string_view line_separator = "\r\n";
    std::vector<std::uint8_t> result{};
    auto sol = body_bytes.begin();
    while (true) {
        auto eol = std::search(sol, body_bytes.end(), line_separator.begin(), line_separator.end());
        if (eol == body_bytes.end())
            throw std::runtime_error("No new line found, corrupted chunk response!");
        std::size_t chunk_size{};
        const auto chunk_size_part = reinterpret_cast<const char*>(&*std::find(sol, eol, ';'));
        const auto [ptr, ec] = std::from_chars(reinterpret_cast<const char*>(&*sol), chunk_size_part, chunk_size, 16);
        if (ec != std::error_code{} || ptr != chunk_size_part)
            throw std::runtime_error("Can't cast chunk size, corrupted chunk!");
        const auto data_part_beginning = eol + line_separator.size();
        if (chunk_size == 0) {
            const auto data_part_ending =
                std::search(data_part_beginning, body_bytes.end(), line_separator.begin(), line_separator.end());
            result.reserve(result.size() + data_part_ending - data_part_beginning);
            result.insert(result.end(), data_part_beginning, data_part_ending);
            break;
        }
        result.reserve(result.size() + chunk_size);
        result.insert(result.end(), data_part_beginning, data_part_beginning + chunk_size);
        sol = data_part_beginning + chunk_size + line_separator.size();
    }
    return result;
}

std::vector<std::string> split(std::string_view str, std::string_view del) {
    std::vector<std::string> result{};
    std::size_t start_pos{0};
    std::size_t end_pos = str.find(del);
    while (end_pos != std::string::npos) {
        result.emplace_back(str.substr(start_pos, end_pos - start_pos));
        start_pos = end_pos + del.size();
        end_pos = str.find(del, start_pos);
    }
    result.emplace_back(str.substr(start_pos));
    return result;
}

std::string trim(std::string_view str) {
    static const std::regex re(R"(^\s+|\s+$)");
    std::string result{};
    std::regex_replace(std::back_inserter(result), str.begin(), str.end(), re, "");
    return result;
}

std::pair<unsigned int, std::string> process_header_status_part(std::string& header_str) {
    const auto first_space = header_str.find(" ");
    const auto second_space = header_str.find(" ", first_space + 1);
    const auto first_eol = header_str.find("\r\n", second_space + 1);
    const auto status_code = unsigned(std::stoi(header_str.substr(first_space + 1, second_space + 1 - first_space)));
    const auto status_text = header_str.substr(second_space + 1, first_eol + 1 - second_space);
    header_str.erase(0, first_eol + 2);
    return std::pair<unsigned int, std::string>{status_code, status_text};
}

std::unordered_map<std::string, std::string> process_headers(std::string& header_str) {
    std::unordered_map<std::string, std::string> result{};
    const auto header_lines = split(header_str, "\r\n");
    for (const auto& header_line : header_lines) {
        const auto spliter = header_line.find(":");
        if (spliter == std::string::npos) {
            continue;
        }
        auto key = trim(header_line.substr(0, spliter));
        auto value = trim(header_line.substr(spliter + 1));
        std::transform(key.begin(), key.end(), key.begin(), [](const auto& c) { return std::tolower(c); });
        std::transform(value.begin(), value.end(), value.begin(), [](const auto& c) { return std::tolower(c); });
        if (const auto founded = result.find(key); founded != result.end()) {
            founded->second = founded->second + "," + value;
        } else {
            result[key] = value;
        }
    }
    return result;
}

Response Response::from_raw_bytes(const std::vector<std::uint8_t>& raw_bytes) {
    const std::string header_delimiter_str{"\r\n\r\n"};
    const std::vector<std::uint8_t> del{header_delimiter_str.begin(), header_delimiter_str.end()};
    const auto end_of_header_it = std::search(raw_bytes.begin(), raw_bytes.end(), del.begin(), del.end());
    if (end_of_header_it == raw_bytes.end()) {
        throw std::runtime_error("Broken response!");
    }
    std::string header_str{raw_bytes.begin(), end_of_header_it};
    const auto status_result = process_header_status_part(header_str);
    const auto headers = process_headers(header_str);
    std::vector<std::uint8_t> body_bytes{end_of_header_it + 4, raw_bytes.end()};
    return Response{status_result.first, std::move(status_result.second), std::move(headers), std::move(body_bytes)};
}

std::vector<std::uint8_t> Response::body_bytes() const {
    if (const auto founded = this->_response_header.find("transfer-encoding");
        founded != _response_header.end() && founded->second.find("chunked") != std::string::npos) {
        return process_chunked_response(this->_body_bytes);
    }
    return this->_body_bytes;
}

std::string Response::body_text() const {
    const auto body_bytes = this->body_bytes();
    return std::string{body_bytes.begin(), body_bytes.end()};
}
void Response::raise_for_status() const {
    if (this->_status_code > 299 || this->_status_code < 200) {
        throw std::runtime_error("Http " + std::to_string(this->_status_code) + " error!");
    }
}

} // namespace TAOW::http_client