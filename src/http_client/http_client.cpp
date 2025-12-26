#include "taow/http_client.hpp"
#include <boost/asio.hpp>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace TAOW::http_client {

Response Client::call() {
    this->_socket.close();
    this->_context.restart();
    this->_ec = std::nullopt;
    this->_raw_result_bytes = std::vector<std::uint8_t>{};
    this->_resolver.async_resolve(
        this->_host, "http",
        [this](const boost::system::error_code& ec, const boost::asio::ip::tcp::resolver::results_type& endpoints) {
            if (ec) {
                this->_ec = ec;
                this->_context.stop();
                return;
            }
            this->_handle_connect(endpoints);
        });
    this->_context.run();
    if (this->_ec)
        throw std::runtime_error(this->_ec->what());
    return Response{std::move(this->_raw_result_bytes)};
}

void Client::_handle_connect(const boost::asio::ip::tcp::resolver::results_type& endpoints) {
    boost::asio::async_connect(
        this->_socket, endpoints,
        [this](const boost::system::error_code& ec, const boost::asio::ip::tcp::endpoint& endpoints) {
            if (ec) {
                this->_ec = ec;
                this->_context.stop();
                return;
            }
            this->_handle_write();
        });
}

void Client::_handle_write() {
    this->_create_raw_request();
    boost::asio::async_write(this->_socket, boost::asio::buffer(this->_raw_request_bytes),
                             [this](const boost::system::error_code& ec, std::size_t transferred) {
                                 if (ec) {
                                     this->_ec = ec;
                                     this->_context.stop();
                                     return;
                                 }
                                 this->_handle_read();
                             });
}

void Client::_handle_read() {
    auto tmp_buff = std::make_shared<std::vector<std::uint8_t>>();
    tmp_buff->resize(8192);
    this->_socket.async_read_some(
        boost::asio::buffer(*tmp_buff), [this, tmp_buff](const boost::system::error_code& ec, size_t transferred) {
            if (!ec) {
                this->_raw_result_bytes.reserve(this->_raw_result_bytes.size() + transferred);
                this->_raw_result_bytes.insert(this->_raw_result_bytes.end(), tmp_buff->begin(),
                                               tmp_buff->begin() + transferred);
                this->_handle_read();
                return;
            }
            if (ec == boost::asio::error::eof) {
                this->_context.stop();
                return;
            }
            this->_ec = ec;
            this->_context.stop();
        });
}

void Client::_create_raw_request() {
    auto header = this->_create_headers();
    auto body = this->_create_body();
    this->_raw_request_bytes.clear();
    this->_raw_request_bytes.reserve(header.size() + body.size());
    this->_raw_request_bytes.insert(this->_raw_request_bytes.end(), header.begin(), header.end());
    this->_raw_request_bytes.insert(this->_raw_request_bytes.end(), body.begin(), body.end());
}

std::vector<std::uint8_t> Client::_create_headers() const {
    std::unordered_map<std::string, std::string> headers{{"Host", this->_host}, {"Connection", "close"}};
    if (this->_json) {
        headers["Content-Type"] = "application/json";
    }
    std::string result{};
    result += HttpMethod_to_string(this->_method);
    result += " " + this->_path + " HTTP/1.1\r\n";
    for (const auto& item : headers) {
        result += item.first + ": " + item.second + "\r\n";
    }
    result += "\r\n";
    return std::vector<std::uint8_t>{result.begin(), result.end()};
}

std::vector<std::uint8_t> Client::_create_body() const {
    if (this->_json) {
        return std::vector<std::uint8_t>{this->_json.value().begin(), this->_json.value().end()};
    }
    return std::vector<std::uint8_t>{};
}

} // namespace TAOW::http_client