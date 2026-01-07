#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
namespace TAOW::cli_builder {

struct CliBuilderOptionBase {
    virtual void from_args(const std::vector<std::string>& args) = 0;
    virtual ~CliBuilderOptionBase() = default;
};

struct CliCommandPack {
    CliCommandPack(std::unique_ptr<CliBuilderOptionBase> data, std::function<int(const CliBuilderOptionBase&)> callable)
        : data(std::move(data)), callable(std::move(callable)) {}
    std::unique_ptr<CliBuilderOptionBase> data;
    std::function<int(const CliBuilderOptionBase& data)> callable;
};

struct CliRegistry {
    static std::unordered_map<std::string, CliCommandPack> commands;

    static int run(int argc, char* argv[]);
};

} // namespace TAOW::cli_builder