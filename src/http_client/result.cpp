#include "taow/http_client.hpp"
#include <algorithm>
#include <cctype>
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

std::string Response::body_text() const {
    const std::string raw_body_str{this->_body_bytes.begin(), this->_body_bytes.end()};
    if (const auto founded = this->_response_header.find("transfer-encoding");
        founded != _response_header.end() && founded->second == "chunked") {
        std::string result{};
        const auto chunks = split(raw_body_str, "\r\n");
        for (int i = 0; i <= chunks.size(); i += 2) {
            if (chunks[i] == "0") {
                break;
            }
            result.append(chunks[i + 1]);
        }
        return result;
    }
    return raw_body_str;
}
void Response::raise_for_status() const {
    if (this->_status_code > 299 || this->_status_code < 200) {
        throw std::runtime_error("Http " + std::to_string(this->_status_code) + " error!");
    }
}

} // namespace TAOW::http_client