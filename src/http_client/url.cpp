#include "taow/http_client.hpp"
#include "taow/utility.hpp"
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

namespace TAOW::http_client {

    std::string URL::build_path() const {
        const auto params = this->_build_params();
        return this->path + params;
    }

    std::string URL::_build_params() const {
        if (this->query_params.empty())
            return "";
        std::vector<std::string> params;
        params.reserve(this->query_params.size());
        std::transform(this->query_params.begin(), this->query_params.end(),
                       std::back_inserter(params),
                       [](const auto& v) -> std::string {
                           return v.first + "=" + v.second;
                       });
        return "?" +
               TAOW::utility::string::join(params.begin(), params.end(), "&");
    }
} // namespace TAOW::http_client