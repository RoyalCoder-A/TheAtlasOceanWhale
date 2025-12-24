#pragma once

#include "taow/http_client/request.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using string = std::string;
namespace TAOW::http_client {
    struct Client {
        explicit Client(std::unique_ptr<Request> request)
            : _request(std::move(request)), _resolver{_context} {}
        Client(const Client& obj) = delete;
        Client(Client&& obj) = delete;
        ~Client() = default;

        void call();
        int status_code() const;
        void raise_for_status() const;
        string text() const;
        std::vector<uint8_t> raw_response() const;

      private:
        std::unique_ptr<Request> _request;
        std::vector<uint8_t> _response_bytes;
        std::unordered_map<string, string> _response_headers;
        unsigned short _status_code;
        boost::asio::ip::tcp::resolver _resolver;
        boost::asio::io_context _context;
    };
} // namespace TAOW::http_client