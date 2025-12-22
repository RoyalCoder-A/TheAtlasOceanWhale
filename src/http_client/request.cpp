#include "taow/http_client.hpp"
#include <string>

using string = std::string;

namespace TAOW::http_client {

    string method_to_string(const HTTPMethod& method) {
        switch (method) {
        case GET:
            return "GET";
        case POST:
            return "POST";
        case OPTION:
            return "OPTION";
        case PUT:
            return "PUT";
        case DELETE:
            return "DELETE";
        }
    }

    Protocol Request::get_protocol() const { return this->_url.protocol; }

    string Request::build_headers() const {
        string result;
        result += method_to_string(this->_method) + " " + this->_url.path +
                  " HTTP/1.1 \r\n";
        for (const auto& item : this->_headers) {
            result += item.first + ": " + item.second + "\r\n";
        }
        result += "\r\n";
        return result;
    }

} // namespace TAOW::http_client