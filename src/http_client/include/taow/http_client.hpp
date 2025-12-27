#pragma once

#include "taow/utils_macros.hpp"
#include <algorithm>
#include <boost/asio.hpp>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace TAOW::http_client {

static const std::string http_header_delimiter = "\r\n\r\n";

#define HTTP_METHOD_ENUM_DEFINITION(X)                                                                                 \
    X(HttpMethod, GET)                                                                                                 \
    X(HttpMethod, POST)                                                                                                \
    X(HttpMethod, PUT)                                                                                                 \
    X(HttpMethod, OPTION)                                                                                              \
    X(HttpMethod, DELETE)                                                                                              \
    X(HttpMethod, PATCH)
CREATE_ENUM_WITH_CASTING(HttpMethod, HTTP_METHOD_ENUM_DEFINITION);
#undef HTTP_METHOD_ENUM_DEFINITION
struct Response {
    Response(std::vector<std::uint8_t> raw_bytes) : _raw_bytes(std::move(raw_bytes)) {
        std::vector<std::uint8_t> del{http_header_delimiter.begin(), http_header_delimiter.end()};
        auto header_last_it = std::search(_raw_bytes.begin(), _raw_bytes.end(), del.begin(), del.end());
        if (header_last_it == _raw_bytes.end()) {
            throw std::runtime_error("Broken response!");
        }
        auto body_first_it = header_last_it + del.size();
    }
    Response(const Response& obj) = delete;
    Response(Response&& obj) = default;
    ~Response() = default;

    void raise_for_status() const;
    std::string body_text() const;

  private:
    const std::vector<std::uint8_t> _raw_bytes;
    const unsigned short _status_code;
    const std::unordered_map<std::string, std::string> _response_header;
    const std::vector<std::uint8_t> _body_bytes;
};

struct Client {
    Client(std::string json, std::string host, std::string path, HttpMethod method)
        : _json(std::move(json)), _host(std::move(host)), _path(std::move(path)), _method(method), _socket(_context),
          _resolver(_context) {}
    Client(std::string host, std::string path, HttpMethod method)
        : _host(std::move(host)), _path(std::move(path)), _method(method), _socket(_context), _resolver(_context) {}
    Client(const Client& obj) = delete;
    Client(Client&& obj) = delete;
    ~Client() = default;

    Response call();

  private:
    std::optional<std::string> _json;
    std::string _host;
    std::string _path;
    HttpMethod _method;
    boost::asio::io_context _context{};
    boost::asio::ip::tcp::socket _socket;
    boost::asio::ip::tcp::resolver _resolver;
    std::vector<std::uint8_t> _raw_result_bytes{};
    std::vector<std::uint8_t> _raw_request_bytes{};
    std::optional<boost::system::error_code> _ec{};

    std::vector<std::uint8_t> _create_headers() const;
    std::vector<std::uint8_t> _create_body() const;
    void _create_raw_request();
    void _handle_connect(const boost::asio::ip::tcp::resolver::results_type& endpoints);
    void _handle_write();
    void _handle_read();
};

} // namespace TAOW::http_client