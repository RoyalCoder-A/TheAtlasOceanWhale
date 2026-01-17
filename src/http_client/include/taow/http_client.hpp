#pragma once

#include "taow/form_encoding.hpp"
#include "taow/multipart.hpp"
#include "taow/url.hpp"
#include "taow/utils_macros.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
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
    Client(URL url, HttpMethod method)
        : _url(std::move(url)), _method(method), _socket(_context), _resolver(_context),
          _ssl_context(boost::asio::ssl::context::sslv23), _ssl_socket(_context, _ssl_context) {
        _ssl_context.set_default_verify_paths();
    }
    Client(std::string json, URL url, HttpMethod method) : Client(std::move(url), method) { _json = std::move(json); }
    Client(FormRequest req, URL url, HttpMethod method) : Client(std::move(url), method) {
        _form_request = std::move(req);
    }
    Client(Multipart req, URL url, HttpMethod method) : Client(std::move(url), method) {
        _multipart.emplace(std::move(req));
    }
    Client(const Client& obj) = delete;
    Client(Client&& obj) = delete;
    ~Client() = default;

    Response call();

  private:
    std::optional<std::string> _json;
    std::optional<FormRequest> _form_request;
    std::optional<Multipart> _multipart;
    URL _url;
    HttpMethod _method;
    boost::asio::io_context _context{};
    boost::asio::ip::tcp::socket _socket;
    boost::asio::ssl::context _ssl_context;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> _ssl_socket;
    boost::asio::ip::tcp::resolver _resolver;
    std::vector<std::uint8_t> _raw_result_bytes{};
    std::vector<std::uint8_t> _raw_request_bytes{};
    std::optional<boost::system::error_code> _ec{};
    std::size_t _content_length;
    std::string _multipart_boundary;

    std::vector<std::uint8_t> _create_headers() const;
    std::vector<std::uint8_t> _create_body();
    void _create_raw_request();
    void _handle_connect(const boost::asio::ip::tcp::resolver::results_type& endpoints);
    void _handle_write();
    void _handle_read();
};

} // namespace TAOW::http_client
