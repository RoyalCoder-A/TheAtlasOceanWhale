#pragma once
#include "taow/cli_builder.hpp"
#include <memory>
#include <string>

#define DEFINE_PROPERTIES(name, arg_option, string_int_double_bool) std::string name;

#define CLI_COMMAND(name, COMMAND_DEFINITION)                                                                          \
    struct name##_data : TAOW::cli_builder::CliBuilderOptionBase {                                                     \
        COMMAND_DEFINITION(DEFINE_PROPERTIES)                                                                          \
        void from_args(const std::vector<std::string>& args);                                                          \
    };                                                                                                                 \
    static int name(const name##_data& data);                                                                          \
    namespace {                                                                                                        \
    struct Register_##name {                                                                                           \
        Register_##name() {                                                                                            \
            auto data_ptr = std::make_unique<name##_data>();                                                           \
            auto wrapper = [](const TAOW::cli_builder::CliBuilderOptionBase& data) {                                   \
                return name(static_cast<const name##_data&>(data));                                                    \
            };                                                                                                         \
            TAOW::cli_builder::CliRegistry::commands[#name] =                                                          \
                TAOW::cli_builder::CliCommandPack{std::move(data_ptr), wrapper};                                       \
        }                                                                                                              \
        static Register_##name registerer_##name##_instance;                                                           \
    };                                                                                                                 \
    }                                                                                                                  \
    static int name(const name##_data& data)