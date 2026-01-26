#include "taow/http_client.hpp"
#include "boost/asio/ssl/verify_mode.hpp"
#include "taow/http_client_exceptions.hpp"
#include "taow/string_utils.hpp"
#include "taow/url.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace TAOW::http_client {

Response Client::call(std::chrono::milliseconds timeout) {
    this->_socket.close();
    this->_context.restart();
    this->_ec = std::nullopt;
    this->_raw_result_bytes = std::vector<std::uint8_t>{};
    _timer.expires_after(timeout);
    _timer.async_wait([this](const boost::system::error_code& ec) {
        if (!ec) {
            this->_ec = boost::asio::error::timed_out;
            boost::system::error_code ignored;
            auto _ = this->_socket.close(ignored);
            auto __ = this->_ssl_socket.lowest_layer().close(ignored);
            this->_resolver.cancel();
        }
    });
    this->_resolver.async_resolve(
        this->_url.get_host(), URLSchema_to_string(this->_url.get_schema()),
        [this](const boost::system::error_code& ec, const boost::asio::ip::tcp::resolver::results_type& endpoints) {
            if (ec) {
                this->_ec = ec;
                this->_context.stop();
                return;
            }
            this->_handle_connect(endpoints);
        });
    this->_context.run();
    _timer.cancel();
    if (this->_ec)
        throw HttpClientError(this->_ec->message());
    return Response::from_raw_bytes(this->_raw_result_bytes);
}

void Client::_handle_connect(const boost::asio::ip::tcp::resolver::results_type& endpoints) {
    boost::asio::async_connect(
        this->_url.get_schema() == URLSchema::http ? this->_socket : this->_ssl_socket.lowest_layer(), endpoints,
        [this](const boost::system::error_code& ec, const boost::asio::ip::tcp::endpoint& endpoints) {
            if (ec) {
                this->_ec = ec;
                this->_context.stop();
                return;
            }
            if (this->_url.get_schema() == URLSchema::https) {
                if (!SSL_set_tlsext_host_name(_ssl_socket.native_handle(), _url.get_host().c_str())) {
                    boost::system::error_code error{static_cast<int>(::ERR_get_error()),
                                                    boost::asio::error::get_ssl_category()};
                    this->_ec = error;
                    this->_context.stop();
                    return;
                }
                this->_ssl_socket.lowest_layer().set_option(boost::asio::ip::tcp::no_delay(true));
                this->_ssl_socket.set_verify_mode(boost::asio::ssl::verify_peer);
                this->_ssl_socket.set_verify_callback(boost::asio::ssl::host_name_verification(this->_url.get_host()));
                this->_ssl_socket.handshake(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>::client);
            }
            this->_handle_write();
        });
}

void Client::_handle_write() {
    this->_create_raw_request();
    auto callback = [this](const boost::system::error_code& ec, std::size_t transferred) {
        if (ec) {
            this->_ec = ec;
            this->_context.stop();
            return;
        }
        this->_handle_read();
    };
    if (this->_url.get_schema() == URLSchema::http)
        boost::asio::async_write(this->_socket, boost::asio::buffer(this->_raw_request_bytes), callback);
    else
        boost::asio::async_write(this->_ssl_socket, boost::asio::buffer(this->_raw_request_bytes), callback);
}

void Client::_handle_read() {
    auto tmp_buff = std::make_shared<std::vector<std::uint8_t>>();
    tmp_buff->resize(8192);

    auto callback = [this, tmp_buff](const boost::system::error_code& ec, size_t transferred) {
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
    };
    if (this->_url.get_schema() == URLSchema::http)
        this->_socket.async_read_some(boost::asio::buffer(*tmp_buff), callback);
    else
        this->_ssl_socket.async_read_some(boost::asio::buffer(*tmp_buff), callback);
}

void Client::_create_raw_request() {
    auto body = this->_create_body();
    this->_content_length = body.size();
    auto header = this->_create_headers();
    this->_raw_request_bytes.clear();
    this->_raw_request_bytes.reserve(header.size() + body.size());
    this->_raw_request_bytes.insert(this->_raw_request_bytes.end(), header.begin(), header.end());
    this->_raw_request_bytes.insert(this->_raw_request_bytes.end(), body.begin(), body.end());
    std::string tmp{this->_raw_request_bytes.begin(), this->_raw_request_bytes.end()};
}

std::vector<std::uint8_t> Client::_create_headers() const {
    std::unordered_map<std::string, std::string> headers{
        {"Host", this->_url.get_host()}, {"Connection", "close"}, {"Accept", "*/*"}, {"Accept-Encoding", "gzip"}};
    if (this->_json) {
        headers["Content-Length"] = std::to_string(this->_content_length);
        headers["Content-Type"] = "application/json";
    }
    if (this->_form_request) {
        headers["Content-Length"] = std::to_string(this->_content_length);
        headers["Content-Type"] = "application/x-www-form-urlencoded";
    }
    if (this->_multipart) {
        headers["Content-Length"] = std::to_string(this->_content_length);
        headers["Content-Type"] = "multipart/form-data; boundary=" + this->_multipart_boundary;
    }
    std::string result{};
    result += HttpMethod_to_string(this->_method);
    result += " " + this->_url.get_path() + " HTTP/1.1\r\n";
    for (const auto& item : headers) {
        result += item.first + ": " + item.second + "\r\n";
    }
    result += "\r\n";
    return std::vector<std::uint8_t>{result.begin(), result.end()};
}

std::vector<std::uint8_t> Client::_create_body() {
    if (this->_json) {
        return std::vector<std::uint8_t>{this->_json.value().begin(), this->_json.value().end()};
    }
    if (this->_form_request) {
        const auto form_value = this->_form_request.value().to_string();
        return std::vector<std::uint8_t>{form_value.begin(), form_value.end()};
    }
    if (this->_multipart) {
        this->_multipart_boundary = utils::random_string(16);
        return this->_multipart.value().as_http_body(this->_multipart_boundary);
    }
    return std::vector<std::uint8_t>{};
}

} // namespace TAOW::http_client
