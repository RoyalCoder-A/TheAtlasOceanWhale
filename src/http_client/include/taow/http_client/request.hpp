#pragma once

#include "taow/http_client/url.hpp"
#include <string>
#include <unordered_map>

namespace TAOW::http_client {

    enum HTTPMethod { GET, POST, OPTION, PUT, DELETE };

    struct Request {
        Request(std::unordered_map<std::string, std::string> headers,
                std::string body, URL url, HTTPMethod method)
            : _headers(std::move(headers)), _body(std::move(body)),
              _url(std::move(url)), _method(method) {}
        Request(std::string body, URL url, HTTPMethod method)
            : _body(std::move(body)), _url(std::move(url)), _method(method) {}
        ~Request() = default;
        Request(const Request& req) = default;
        Request(Request&& req) = default;
        std::string build_headers() const;
        Protocol get_protocol() const;

      private:
        std::unordered_map<std::string, std::string> _headers;
        std::string _body;
        URL _url;
        HTTPMethod _method;
    };

} // namespace TAOW::http_client