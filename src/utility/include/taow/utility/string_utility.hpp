#pragma once

#include <string>
#include <string_view>

namespace TAOW::utility::string {
    template <typename It>
    std::string join(It begin, It end, std::string_view del) {
        std::string out;
        if (begin == end)
            return out;
        out += *begin;
        ++begin;
        while (begin != end) {
            out += del;
            out += *begin;
            ++begin;
        }
        return out;
    }
} // namespace TAOW::utility::string