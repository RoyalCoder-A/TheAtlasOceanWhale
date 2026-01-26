#include "taow/http_client.hpp"
#include "taow/http_client_exceptions.hpp"
#include "taow/utils_macros.hpp"
#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <regex>
#include <string>
#include <string_view>
#include <system_error>
#include <unordered_map>
#include <utility>
#include <vector>
#include <zlib.h>

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

#define COMPRESS_ALGOS_ENUM_DEFINITION(X)                                                                              \
    X(CompressAlgos, gzip)                                                                                             \
    X(CompressAlgos, identity)

CREATE_ENUM_WITH_CASTING(CompressAlgos, COMPRESS_ALGOS_ENUM_DEFINITION);
#undef COMPRESS_ALGOS_ENUM_DEFINITION

std::vector<std::uint8_t> gunzip(const std::vector<std::uint8_t>& input) {
    if (input.empty())
        return {};

    z_stream zs{};
    zs.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(input.data()));
    zs.avail_in = static_cast<uInt>(input.size());
    int rc = inflateInit2(&zs, 16 + MAX_WBITS);
    if (rc != Z_OK)
        throw DecompressError("inflateInit2 failed");

    std::vector<std::uint8_t> out;
    out.reserve(input.size() * 2);
    std::uint8_t buffer[64 * 1024];
    do {
        zs.next_out = reinterpret_cast<Bytef*>(buffer);
        zs.avail_out = static_cast<uInt>(sizeof(buffer));
        rc = inflate(&zs, Z_NO_FLUSH);
        if (rc != Z_OK && rc != Z_STREAM_END) {
            inflateEnd(&zs);
            throw DecompressError("inflate failed");
        }
        std::size_t produced = sizeof(buffer) - zs.avail_out;
        out.insert(out.end(), buffer, buffer + produced);

    } while (rc != Z_STREAM_END);

    inflateEnd(&zs);
    return out;
}

std::vector<std::uint8_t> handle_decompressing(const std::unordered_map<std::string, std::string>& headers,
                                               const std::vector<std::uint8_t>& input) {
    std::vector<std::uint8_t> result{input};
    if (headers.find("content-encoding") == headers.end())
        return result;
    const auto encodings = split(headers.at("content-encoding"), ",");
    for (auto i = encodings.rbegin(); i != encodings.rend(); ++i) {
        auto encoding_enum = CompressAlgos_from_string(trim(*i));
        if (!encoding_enum)
            throw DecompressError("Can't decompress " + *i);
        switch (encoding_enum.value()) {
        case gzip:
            result = gunzip(result);
            break;
        case identity:
            break;
        }
    }
    return result;
}

std::vector<std::uint8_t> process_chunked_response(const std::vector<uint8_t>& body_bytes) {
    const std::string_view line_separator = "\r\n";
    std::vector<std::uint8_t> result{};
    auto sol = body_bytes.begin();
    while (true) {
        auto eol = std::search(sol, body_bytes.end(), line_separator.begin(), line_separator.end());
        if (eol == body_bytes.end())
            throw BrokenChunkError("No new line found, corrupted chunk response!");
        std::size_t chunk_size{};
        const auto chunk_size_part = reinterpret_cast<const char*>(&*std::find(sol, eol, ';'));
        const auto [ptr, ec] = std::from_chars(reinterpret_cast<const char*>(&*sol), chunk_size_part, chunk_size, 16);
        if (ec != std::error_code{} || ptr != chunk_size_part)
            throw BrokenChunkError("Can't cast chunk size, corrupted chunk!");
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
        throw BrokenResponseError("Broken response!");
    }
    std::string header_str{raw_bytes.begin(), end_of_header_it};
    const auto status_result = process_header_status_part(header_str);
    const auto headers = process_headers(header_str);
    std::vector<std::uint8_t> body_bytes{end_of_header_it + 4, raw_bytes.end()};
    return Response{status_result.first, std::move(status_result.second), std::move(headers), std::move(body_bytes)};
}

std::vector<std::uint8_t> Response::body_bytes() const {
    auto result = this->_body_bytes;
    if (const auto founded = this->_response_header.find("transfer-encoding");
        founded != _response_header.end() && founded->second.find("chunked") != std::string::npos) {
        result = process_chunked_response(result);
    }
    return handle_decompressing(this->_response_header, result);
}

std::string Response::body_text() const {
    const auto body_bytes = this->body_bytes();
    return std::string{body_bytes.begin(), body_bytes.end()};
}
void Response::raise_for_status() const {
    if (this->_status_code > 299 || this->_status_code < 200) {
        throw HttpError("Http " + std::to_string(this->_status_code) + " error!", this->_status_code);
    }
}

} // namespace TAOW::http_client
