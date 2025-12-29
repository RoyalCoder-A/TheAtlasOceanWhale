#include "taow/url.hpp"

namespace TAOW::http_client {

const std::string& URL::get_host() const { return this->_host; }

std::string URL::get_path() const {
    std::string base_path{this->_path};
    if (this->_query_params) {
        base_path.append("?" + this->_query_params.value().to_string());
    }
    return base_path;
}

} // namespace TAOW::http_client