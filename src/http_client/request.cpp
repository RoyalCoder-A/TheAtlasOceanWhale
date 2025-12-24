#include "taow/http_client.hpp"
#include <boost/asio.hpp>
#include <string>
#include <unordered_map>

using string = std::string;
namespace TAOW::http_client {

    std::unordered_map<string, string> JsonRequest::get_headers() const {
        std::unordered_map<string, string> headers{this->_headers};
        headers["Content-Type"] = "application/json";
        return headers;
    }

} // namespace TAOW::http_client