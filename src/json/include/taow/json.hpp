#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

#define SET_SCALAR_PROPERTY(type, name) type name{};
#define SET_LIST_PROPERTY(type, name) std::vector<type> name{};

#define SET_PROPERTY(type, name, list_scalar) SET_##list_scalar##_PROPERTY(type, name)

#define FILL_SCALAR_PROPERTY(type, name) obj.name = std::move(json_data[#name]);
#define FILL_LIST_PROPERTY(type, name) obj.name = json_data.at(#name).get<std::vector<type>>();

#define FILL_PROPERTY(type, name, list_scalar) FILL_##list_scalar##_PROPERTY(type, name)

#define CREATE_STRUCT_WITH_FROM_JSON(struct_name, INIT_DEFINITIONS)                                                    \
    struct struct_name {                                                                                               \
        INIT_DEFINITIONS(SET_PROPERTY)                                                                                 \
        static struct_name from_json_str(std::string json_str) {                                                       \
            auto json_data = json::parse(json_str);                                                                    \
            return struct_name::from_json(json_data);                                                                  \
        }                                                                                                              \
        static struct_name from_json(json json_data) {                                                                 \
            struct_name obj{};                                                                                         \
            INIT_DEFINITIONS(FILL_PROPERTY)                                                                            \
            return obj;                                                                                                \
        }                                                                                                              \
    }