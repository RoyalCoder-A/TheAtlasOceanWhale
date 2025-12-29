#pragma once

#include "taow/form_encoding.hpp"
#include "taow/utils_macros.hpp"
#include <boost/asio.hpp>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
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
#undef HTTP_METHOD_ENUM_DEFINITION
struct Response {
    Response(unsigned int status_code, std::string status_text,
             std::unordered_map<std::string, std::string> response_header, std::vector<std::uint8_t> body_bytes)
        : _status_code(status_code), _status_text(std::move(status_text)), _response_header(std::move(response_header)),
          _body_bytes(std::move(body_bytes)) {}
    Response(const Response& obj) = delete;
    Response(Response&& obj) = default;
    ~Response() = default;

    void raise_for_status() const;
    std::string body_text() const;
    std::vector<std::uint8_t> body_bytes() const;

    static Response from_raw_bytes(const std::vector<std::uint8_t>& raw_bytes);

  private:
    const unsigned int _status_code;
    const std::string _status_text;
    const std::unordered_map<std::string, std::string> _response_header;
    const std::vector<std::uint8_t> _body_bytes;
};

struct Client {
    Client(std::string json, std::string host, std::string path, HttpMethod method)
        : _json(std::move(json)), _host(std::move(host)), _path(std::move(path)), _method(method), _socket(_context),
          _resolver(_context) {}
    Client(FormRequest req, std::string host, std::string path, HttpMethod method)
        : _form_request(std::move(req)), _host(std::move(host)), _path(std::move(path)), _method(method),
          _socket(_context), _resolver(_context) {}
    Client(std::string host, std::string path, HttpMethod method)
        : _host(std::move(host)), _path(std::move(path)), _method(method), _socket(_context), _resolver(_context) {}
    Client(const Client& obj) = delete;
    Client(Client&& obj) = delete;
    ~Client() = default;

    Response call();

  private:
    std::optional<std::string> _json;
    std::optional<FormRequest> _form_request;
    std::string _host;
    std::string _path;
    HttpMethod _method;
    boost::asio::io_context _context{};
    boost::asio::ip::tcp::socket _socket;
    boost::asio::ip::tcp::resolver _resolver;
    std::vector<std::uint8_t> _raw_result_bytes{};
    std::vector<std::uint8_t> _raw_request_bytes{};
    std::optional<boost::system::error_code> _ec{};
    std::size_t _content_length;

    std::vector<std::uint8_t> _create_headers() const;
    std::vector<std::uint8_t> _create_body() const;
    void _create_raw_request();
    void _handle_connect(const boost::asio::ip::tcp::resolver::results_type& endpoints);
    void _handle_write();
    void _handle_read();
};

} // namespace TAOW::http_client