#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace TAOW::http_client {
enum HttpMethod { GET, POST, PUT, OPTION, DELETE, PATCH };
struct Response {
    Response(std::vector<uint8_t> raw_bytes, unsigned short status_code)
        : _raw_bytes(std::move(raw_bytes)), _status_code(status_code) {}
    Response(const Response& obj) = delete;
    Response(Response&& obj) = default;
    ~Response() = default;

    void raise_for_status() const;
    std::string text() const;

  private:
    const std::vector<std::uint8_t> _raw_bytes;
    const unsigned short _status_code;
};

struct Client {
    Client(std::string json, std::string host, std::string path, HttpMethod method)
        : _json(std::move(json)), _host(std::move(host)), _path(std::move(path)), _method(method) {}
    Client(std::string host, std::string path, HttpMethod method)
        : _host(std::move(host)), _path(std::move(path)), _method(method) {}
    Client(const Client& obj) = delete;
    Client(Client&& obj) = delete;
    ~Client() = default;

    Response call() const;

  private:
    const std::optional<std::string> _json;
    const std::string _host;
    const std::string _path;
    const HttpMethod _method;
};

} // namespace TAOW::http_client