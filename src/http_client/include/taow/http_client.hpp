#pragma once

#include "taow/utils_macros.hpp"
#include <boost/asio.hpp>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace TAOW::http_client {

#define HTTP_METHOD_ENUM_DEFINITION(X)                                                                                 \
    X(HttpMethod, GET)                                                                                                 \
    X(HttpMethod, POST)                                                                                                \
    X(HttpMethod, PUT)                                                                                                 \
    X(HttpMethod, OPTION)                                                                                              \
    X(HttpMethod, DELETE)                                                                                              \
    X(HttpMethod, PATCH)
CREATE_ENUM_WITH_CASTING(HttpMethod, HTTP_METHOD_ENUM_DEFINITION);
struct Response {
    Response(std::vector<uint8_t> raw_bytes) : _raw_bytes(std::move(raw_bytes)) {}
    Response(const Response& obj) = delete;
    Response(Response&& obj) = default;
    ~Response() = default;

    void raise_for_status() const;
    std::string text() const;

  private:
    const std::vector<std::uint8_t> _raw_bytes;
    unsigned short _status_code;
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