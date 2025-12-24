#pragma once

#include "taow/http_client/url.hpp"
#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <unordered_map>

using string = std::string;
namespace TAOW::http_client {

    enum HTTPMethod { GET, POST, OPTION, PUT, DELETE };

    struct Request {
        Request(URL url, HTTPMethod method,
                std::unordered_map<string, string> headers)
            : _url(std::move(url)), _method(method),
              _headers(std::move(headers)) {}
        Request(const Request& obj) = delete;
        Request(Request&& obj) = default;
        virtual ~Request() = default;

        virtual boost::asio::const_buffer create_request_buffer() const = 0;

      protected:
        std::unordered_map<string, string> _headers;

      private:
        const URL _url;
        const HTTPMethod _method;
    };

    struct JsonRequest : Request {
        explicit JsonRequest(URL url, HTTPMethod method,
                             std::unordered_map<string, string> headers,
                             std::unique_ptr<string> json_string)
            : Request(std::move(url), method, std::move(headers)),
              _json_string{std::move(json_string)} {}
        JsonRequest(const JsonRequest& obj) = delete;
        JsonRequest(JsonRequest&& obj) = default;
        ~JsonRequest() = default;

        std::unordered_map<string, string> get_headers() const;

      private:
        std::unique_ptr<string> _json_string;
    };

} // namespace TAOW::http_client