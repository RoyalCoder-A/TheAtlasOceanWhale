#include "taow/multipart.hpp"
#include <string>
#include <string_view>

namespace TAOW::http_client {

const std::vector<std::uint8_t> Multipart::as_http_body(std::string_view boundary) const {
    std::string boundary_tmp{boundary};
    const std::string crlf = "\r\n";
    std::vector<std::uint8_t> result;
    std::string fields_result;
    for (const auto& [key, val] : this->_fields) {
        fields_result.append("--" + boundary_tmp + crlf);
        fields_result.append("Content-Disposition: form-data; name=\"" + key + "\"" + crlf);
        fields_result.append(crlf);
        fields_result.append(val + crlf);
    }
    result.reserve(fields_result.size());
    result.insert(result.end(), fields_result.begin(), fields_result.end());
    for (const auto& [key, val] : this->_files) {
        std::string str_content;
        str_content.append("--" + boundary_tmp + crlf);
        str_content.append("Content-Disposition: form-data; name=\"" + key + "\"; filename=\"" + val.name + "\"" +
                           crlf);
        str_content.append("Content-Type: " + val.content_type + crlf);
        str_content.append(crlf);
        result.reserve(result.size() + str_content.size() + val.bytes.size() + crlf.size());
        result.insert(result.end(), str_content.begin(), str_content.end());
        result.insert(result.end(), val.bytes.begin(), val.bytes.end());
        result.insert(result.end(), crlf.begin(), crlf.end());
    }
    std::string final_boundary{"--" + boundary_tmp + "--" + crlf};
    result.reserve(final_boundary.size() + result.size());
    result.insert(result.end(), final_boundary.begin(), final_boundary.end());
    return result;
}

} // namespace TAOW::http_client