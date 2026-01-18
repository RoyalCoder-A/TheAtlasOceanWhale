#pragma once

#include <string_view>
namespace TAOW::utils {
template <typename T> constexpr std::string_view get_type_name() {
#if defined(__clang__) || defined(__GNUC__)
    std::string_view name = __PRETTY_FUNCTION__;
    auto start = name.find("T = ") + 4;
    auto end = name.find_last_of(']');
    return name.substr(start, end - start);
#elif defined(_MSC_VER)
    std::string_view name = __FUNCSIG__;
    auto start = name.find('<') + 1;
    auto end = name.find_last_of('>');
    return name.substr(start, end - start);
}
#else
    return "Unknown";
#endif
}
} // namespace TAOW::utils
