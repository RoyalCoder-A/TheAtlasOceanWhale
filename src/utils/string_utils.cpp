#include "taow/string_utils.hpp"
#include <cstddef>
#include <random>
#include <string>
#include <string_view>

namespace TAOW::utils {

std::string random_string(std::size_t length) {
    static constexpr std::string_view chars = "0123456789"
                                              "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                              "abcdefghijklmnopqrstuvwxyz";
    thread_local std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<std::size_t> dist(0, chars.size() - 1);
    std::string result;
    result.reserve(length);
    for (std::size_t i = 0; i < length; ++i)
        result.push_back(chars[dist(rng)]);
    return result;
}

} // namespace TAOW::utils