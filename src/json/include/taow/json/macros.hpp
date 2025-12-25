#pragma once
#include "nlohmann/json.hpp"
#include <algorithm>
#include <iterator>
#include <optional>
#include <string>

using json = nlohmann::json;

#define FIELD_SCALAR(type) type
#define FIELD_LIST(type) std::vector<type>
#define FIELD_OPTIONAL(type, name) std::optional<type> name{};
#define FIELD_REQUIRED(type, name) type name{};

#define CREATE_STRUCT_FIELDS(type, name, list_or_scalar, optional_or_required)                                         \
    FIELD_##optional_or_required(FIELD_##list_or_scalar(type), name)

#define HANDLE_SCALAR_REQUIRED(type, name) obj.name = data[#name];
#define HANDLE_SCALAR_OPTIONAL(type, name)                                                                             \
    if (data.contains(#name) && !data.at(#name).is_null())                                                             \
        obj.name = data.at(#name).get<type>();
#define HANDLE_LIST_REQUIRED(type, name)                                                                               \
    std::transform(data[#name].begin(), data[#name].end(), std::back_inserter(obj.name),                               \
                   [](const auto& item) { return type::from_json(item); });
#define HANDLE_LIST_OPTIONAL(type, name)                                                                               \
    if (data.contains(#name) && !data.at(#name).is_null() && data.at(#name).is_array())                                \
        std::transform(data[#name].begin(), data[#name].end(), std::back_inserter(obj.name),                           \
                       [](const auto& item) { return type::from_json(item); });

#define FILL_STRUCT_FIELDS(type, name, list_or_scalar, optional_or_required)                                           \
    HANDLE_##list_or_scalar##_##optional_or_required(type, name)

#define CREATE_DESERIALIZABLE_STRUCT(struct_name, STRUCT_DEFINITION)                                                   \
    struct struct_name {                                                                                               \
        STRUCT_DEFINITION(CREATE_STRUCT_FIELDS)                                                                        \
        static struct_name from_json_str(std::string data_str) {                                                       \
            auto data = json::parse(std::move(data_str));                                                              \
            return struct_name::from_json(std::move(data));                                                            \
        }                                                                                                              \
                                                                                                                       \
        static struct_name from_json(json data) {                                                                      \
            struct_name obj{};                                                                                         \
            STRUCT_DEFINITION(FILL_STRUCT_FIELDS)                                                                      \
            return obj;                                                                                                \
        }                                                                                                              \
    };