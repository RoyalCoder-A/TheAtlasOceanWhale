#include "taow/http_client.hpp"
#include <cctype>
#include <string>
#include <string_view>

namespace TAOW::http_client {

std::string encode_url(std::string_view input) {
    constexpr char HEX[] = "0123456789ABCDEF";
    std::string output{};
    output.reserve(input.size() * 3);
    for (const unsigned char c : input) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '*') {
            output.push_back(static_cast<char>(c));
        } else if (c == ' ') {
            output.push_back('+');
        } else {
            output.push_back('%');
            output.push_back(HEX[(c >> 4) & 0x0f]);
            output.push_back(HEX[c & 0x0f]);
        }
    }
    return output;
}

} // namespace TAOW::http_client