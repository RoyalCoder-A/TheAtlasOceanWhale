#pragma once

#include "taow/form_encoding.hpp"
#include <optional>
#include <string>
namespace TAOW::http_client {

struct URL {
    URL(std::string host, std::string path) : _host(std::move(host)), _path(std::move(path)) {}
    URL(std::string host, std::string path, FormRequest query_params)
        : _host(std::move(host)), _path(std::move(path)), _query_params(std::move(query_params)) {}

    const std::string& get_host() const;
    std::string get_path() const;

  private:
    const std::string _host;
    const std::string _path;
    const std::optional<FormRequest> _query_params;
};

} // namespace TAOW::http_client