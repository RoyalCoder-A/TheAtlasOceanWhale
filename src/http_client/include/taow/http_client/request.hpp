#pragma once

#include "taow/http_client/url.hpp"
#include <boost/asio.hpp>
#include <string>
#include <unordered_map>

using string = std::string;
namespace TAOW::http_client {

    enum HTTPMethod { GET, POST, OPTION, PUT, DELETE };

    struct Request {
        Request(URL url, HTTPMethod method,
                std::pmr::unordered_map<string, string> headers)
            : _url(std::move(url)), _method(method),
              _headers(std::move(headers)) {}
        Request(const Request& obj) = delete;
        Request(Request&& obj) = default;
        ~Request() = default;

        virtual boost::asio::const_buffer create_request_buffer() const = 0;

      private:
        const URL _url;
        const HTTPMethod _method;
        std::pmr::unordered_map<string, string> _headers;
    };

    struct JsonRequest : Request {
        JsonRequest(URL url, HTTPMethod method,
                    std::pmr::unordered_map<string, string> headers,
                    string json_string)
            : Request(url, method, headers),
              _json_string{std::move(json_string)} {
            headers["Content-Type"] = "application/json";
        }
        JsonRequest(const JsonRequest& obj) = delete;
        JsonRequest(JsonRequest&& obj) = default;
        ~JsonRequest() = default;

        boost::asio::const_buffer create_request_buffer() const override;

      private:
        const string _json_string;
    };

} // namespace TAOW::http_client