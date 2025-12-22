#pragma once

#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>

namespace TAOW::utility::string {

    template <std::input_iterator It>
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

    template <std::input_iterator T>
    void split(std::string_view str, std::string_view del, T result_it) {
        size_t start_pos(0);
        while (true) {
            auto next_pos = str.find(del);
            if (next_pos == std::string::npos) {
                *result_it = str.substr(start_pos);
                return;
            }
            *result_it = str.substr(start_pos, next_pos - start_pos);
            ++result_it;
        }
    }
} // namespace TAOW::utility::string