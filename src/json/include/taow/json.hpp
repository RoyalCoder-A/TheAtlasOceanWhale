#pragma once

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

using json = nlohmann::json;

#define SET_SCALAR_PROPERTY_REQUIRED(type, name) type name{};
#define SET_LIST_PROPERTY_REQUIRED(type, name) std::vector<type> name{};
#define SET_OBJ_PROPERTY_REQUIRED(type, name) type name{};
#define SET_LISTOBJ_PROPERTY_REQUIRED(type, name) std::vector<type> name{};

#define SET_SCALAR_PROPERTY_OPTIONAL(type, name) std::optional<type> name{};
#define SET_LIST_PROPERTY_OPTIONAL(type, name) std::optional<std::vector<type>> name{};
#define SET_OBJ_PROPERTY_OPTIONAL(type, name) std::optional<type> name{};
#define SET_LISTOBJ_PROPERTY_OPTIONAL(type, name) std::optional<std::vector<type>> name{};

#define SET_PROPERTY(type, name, list_scalar_obj_listobj, optional_required)                                           \
    SET_##list_scalar_obj_listobj##_PROPERTY_##optional_required(type, name)

#define FILL_SCALAR_PROPERTY_REQUIRED(type, name) obj.name = std::move(json_data[#name]);
#define FILL_LIST_PROPERTY_REQUIRED(type, name) obj.name = json_data.at(#name).get<std::vector<type>>();
#define FILL_OBJ_PROPERTY_REQUIRED(type, name) obj.name = type::from_json(json_data[#name]);
#define FILL_LISTOBJ_PROPERTY_REQUIRED(type, name)                                                                     \
    for (const auto& item : json_data[#name]) {                                                                        \
        obj.name.push_back(type::from_json(std::move(item)));                                                          \
    }

#define FILL_SCALAR_PROPERTY_OPTIONAL(type, name)                                                                      \
    obj.name = json_data.contains(#name) && !json_data.at(#name).is_null()                                             \
                   ? std::optional<type>{std::move(json_data[#name])}                                                  \
                   : std::nullopt;
#define FILL_LIST_PROPERTY_OPTIONAL(type, name)                                                                        \
    obj.name = json_data.contains(#name) && !json_data.at(#name).is_null()                                             \
                   ? std::optional<std::vector<type>>{json_data.at(#name).get<std::vector<type>>()}                    \
                   : std::nullopt;
#define FILL_OBJ_PROPERTY_OPTIONAL(type, name)                                                                         \
    obj.name = json_data.contains(#name) && !json_data.at(#name).is_null()                                             \
                   ? std::optional<type>{type::from_json(json_data[#name])}                                            \
                   : std::nullopt;
#define FILL_LISTOBJ_PROPERTY_OPTIONAL(type, name)                                                                     \
    if (json_data.contains(#name) && !json_data.at(#name).is_null()) {                                                 \
        std::vector<type> tmp_##name{};                                                                                \
        for (const auto& item : json_data[#name]) {                                                                    \
            tmp_##name.push_back(type::from_json(std::move(item)));                                                    \
        }                                                                                                              \
        obj.name = std::optional<std::vector<type>>{std::move(tmp_##name)};                                            \
    }

#define FILL_PROPERTY(type, name, list_scalar_obj_listobj, optional_required)                                          \
    FILL_##list_scalar_obj_listobj##_PROPERTY_##optional_required(type, name)

#define CREATE_STRUCT_WITH_FROM_JSON(struct_name, INIT_DEFINITIONS)                                                    \
    struct struct_name {                                                                                               \
        INIT_DEFINITIONS(SET_PROPERTY)                                                                                 \
        static struct_name from_json_str(std::string json_str) {                                                       \
            auto json_data = json::parse(json_str);                                                                    \
            return struct_name::from_json(std::move(json_data));                                                       \
        }                                                                                                              \
        static struct_name from_json(json json_data) {                                                                 \
            struct_name obj{};                                                                                         \
            INIT_DEFINITIONS(FILL_PROPERTY)                                                                            \
            return obj;                                                                                                \
        }                                                                                                              \
    }