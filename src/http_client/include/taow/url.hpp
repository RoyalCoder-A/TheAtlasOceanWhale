#pragma once

#include "taow/form_encoding.hpp"
#include "taow/utils_macros.hpp"
#include <optional>
#include <string>
namespace TAOW::http_client {

#define URL_SCHEMA_ENUM_DEFINITION(X)                                                                                  \
    X(URLSchema, http)                                                                                                 \
    X(URLSchema, https)

CREATE_ENUM_WITH_CASTING(URLSchema, URL_SCHEMA_ENUM_DEFINITION);
#undef URL_SCHEMA_ENUM_DEFINITION

struct URL {
    URL(URLSchema schema, std::string host, std::string path)
        : _host(std::move(host)), _path(std::move(path)), _url_schema(schema) {}
    URL(URLSchema schema, std::string host, std::string path, FormRequest query_params)
        : _host(std::move(host)), _path(std::move(path)), _query_params(std::move(query_params)), _url_schema(schema) {}

    const std::string& get_host() const;
    std::string get_path() const;
    URLSchema get_schema() const;

  private:
    const std::string _host;
    const std::string _path;
    const URLSchema _url_schema;
    const std::optional<FormRequest> _query_params;
};

} // namespace TAOW::http_client
