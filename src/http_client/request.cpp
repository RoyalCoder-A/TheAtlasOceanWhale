#include "taow/http_client.hpp"
#include <boost/asio.hpp>

namespace TAOW::http_client {

    boost::asio::const_buffer JsonRequest::create_request_buffer() const {
        return boost::asio::buffer(this->_json_string);
    }

} // namespace TAOW::http_client