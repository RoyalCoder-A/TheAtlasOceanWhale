#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
namespace TAOW::cli_builder {

template <typename T>
std::optional<T> get_option(std::string_view option_name,
                            const std::unordered_map<std::string, std::string>& raw_options);
template <typename T> T get_arg(std::size_t idx, const std::vector<std::string>& args);

struct CliBuilderOptionBase {
    virtual void from_options(const std::unordered_map<std::string, std::string>& options) = 0;
    virtual ~CliBuilderOptionBase() = default;
};

struct CliBuilderArgBase {
    virtual void from_args(const std::vector<std::string>& args) = 0;
    virtual ~CliBuilderArgBase() = default;
};

struct CliCommandPack {
    CliCommandPack(std::unique_ptr<CliBuilderOptionBase> options, std::unique_ptr<CliBuilderArgBase> args,
                   std::function<int(const CliBuilderOptionBase&, const CliBuilderArgBase&)> callable)
        : options(std::move(options)), args(std::move(args)), callable(std::move(callable)) {}
    std::unique_ptr<CliBuilderOptionBase> options;
    std::unique_ptr<CliBuilderArgBase> args;
    std::function<int(const CliBuilderOptionBase& options, const CliBuilderArgBase& args)> callable;
};

struct CliRegistry {
    inline static std::unordered_map<std::string, CliCommandPack> commands;

    static int run(int argc, char* argv[]);
};

} // namespace TAOW::cli_builder
