#pragma once
#include "taow/cli_builder.hpp"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#define DEFINE_STRING_OPTION(name) std::optional<std::string> name;
#define DEFINE_INT_OPTION(name) std::optional<int> name;
#define DEFINE_DOUBLE_OPTION(name) std::optional<double> name;
#define DEFINE_BOOL_OPTION(name) std::optional<bool> name;
#define DEFINE_OPTIONS(name, string_int_double_bool) DEFINE_##string_int_double_bool##_OPTION(name)

#define EXTRACT_STRING_OPTION(name) this->name = TAOW::cli_builder::get_option<std::string>(#name, options);
#define EXTRACT_INT_OPTION(name) this->name = TAOW::cli_builder::get_option<int>(#name, options);
#define EXTRACT_DOUBLE_OPTION(name) this->name = TAOW::cli_builder::get_option<double>(#name, options);
#define EXTRACT_BOOL_OPTION(name) this->name = TAOW::cli_builder::get_option<bool>(#name, options);
#define OPTION_EXTRACTOR(name, string_int_double_bool) EXTRACT_##string_int_double_bool##_OPTION(name)

#define DEFINE_STRING_ARG(name) std::string name;
#define DEFINE_INT_ARG(name) int name;
#define DEFINE_DOUBLE_ARG(name) double name;
#define DEFINE_BOOL_ARG(name) bool name;
#define DEFINE_ARGS(name, idx, string_int_double_bool) DEFINE_##string_int_double_bool##_ARG(name)

#define EXTRACT_STRING_ARG(name, idx) this->name = TAOW::cli_builder::get_arg<std::string>(idx, args);
#define EXTRACT_INT_ARG(name, idx) this->name = TAOW::cli_builder::get_arg<int>(idx, args);
#define EXTRACT_DOUBLE_ARG(name, idx) this->name = TAOW::cli_builder::get_arg<double>(idx, args);
#define EXTRACT_BOOL_ARG(name, idx) this->name = TAOW::cli_builder::get_arg<bool>(idx, args);
#define ARG_EXTRACTOR(name, idx, string_int_double_bool) EXTRACT_##string_int_double_bool##_ARG(name, idx)

#define CLI_COMMAND(name, OPTION_DEFINITION, ARG_DEFINITION)                                                           \
    struct name##_options : TAOW::cli_builder::CliBuilderOptionBase {                                                  \
        OPTION_DEFINITION(DEFINE_OPTIONS)                                                                              \
        void from_options(const std::unordered_map<std::string, std::string>& options) override {                      \
            OPTION_DEFINITION(OPTION_EXTRACTOR)                                                                        \
        }                                                                                                              \
    };                                                                                                                 \
    struct name##_args : TAOW::cli_builder::CliBuilderArgBase {                                                        \
        ARG_DEFINITION(DEFINE_ARGS)                                                                                    \
        void from_args(const std::vector<std::string>& args) override { ARG_DEFINITION(ARG_EXTRACTOR) }                \
    };                                                                                                                 \
    static int name(const name##_options& options, const name##_args& args);                                           \
    namespace {                                                                                                        \
    struct Register_##name {                                                                                           \
        Register_##name() {                                                                                            \
            auto options_ptr = std::make_unique<name##_options>();                                                     \
            auto args_ptr = std::make_unique<name##_args>();                                                           \
            auto wrapper = [this](const TAOW::cli_builder::CliBuilderOptionBase& options,                              \
                                  const TAOW::cli_builder::CliBuilderArgBase& args) {                                  \
                return name(static_cast<const name##_options&>(options), static_cast<const name##_args&>(args));       \
            };                                                                                                         \
            TAOW::cli_builder::CliRegistry::commands.try_emplace(#name, std::move(options_ptr), std::move(args_ptr),   \
                                                                 std::move(wrapper));                                  \
        }                                                                                                              \
    };                                                                                                                 \
    static Register_##name registerer_##name##_instance;                                                               \
    }                                                                                                                  \
    static int name(const name##_options& options, const name##_args& args)
