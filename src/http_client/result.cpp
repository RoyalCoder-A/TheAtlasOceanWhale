#include "taow/http_client.hpp"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace TAOW::http_client {

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
        if (header_line.find(":") == std::string::npos) {
            continue;
        }
        const auto key_val = split(header_line, ":");
        result[trim(key_val[0])] = trim(key_val[1]);
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

std::string Response::body_text() const { return std::string{this->_body_bytes.begin(), this->_body_bytes.end()}; }
void Response::raise_for_status() const {
    if (this->_status_code > 299 || this->_status_code < 200) {
        throw std::runtime_error("Http " + std::to_string(this->_status_code) + " error!");
    }
}

} // namespace TAOW::http_client