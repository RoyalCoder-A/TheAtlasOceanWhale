#pragma once
#include <optional>
#include <string_view>

#define SET_ENUM_PROPERTIES(type, name) name,
#define ENUM_TO_STRING_CASES(type, name)                                                                               \
    case type::name:                                                                                                   \
        return #name;
#define ENUM_FROM_STRING_CASES(type, name)                                                                             \
    if (str == #name) {                                                                                                \
        return std::optional<type>{type::name};                                                                        \
    }

#define CREATE_ENUM_WITH_CASTING(enum_name, ENUM_DEFINITION)                                                           \
    enum enum_name { ENUM_DEFINITION(SET_ENUM_PROPERTIES) };                                                           \
    inline constexpr std::string_view enum_name##_to_string(enum_name obj) noexcept {                                  \
        switch (obj) { ENUM_DEFINITION(ENUM_TO_STRING_CASES) }                                                         \
        return "unknown";                                                                                              \
    }                                                                                                                  \
    inline std::optional<enum_name> enum_name##_from_string(std::string_view str) noexcept {                           \
        ENUM_DEFINITION(ENUM_FROM_STRING_CASES)                                                                        \
        return std::nullopt;                                                                                           \
    }