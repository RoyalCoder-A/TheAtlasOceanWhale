#pragma once

#include <map>
#include <string>
namespace TAOW::http_client {

    enum Protocol { HTTP, HTTPS };

    struct URL {

        ~URL() = default;
        URL(std::string host, std::string path, Protocol protocol,
            std::pmr::map<std::string, std::string> query_params)
            : host(std::move(host)), path(std::move(path)), protocol(protocol),
              query_params(std::move(query_params)) {}
        URL(std::string host, std::string path, Protocol protocol)
            : host(std::move(host)), path(std::move(path)), protocol(protocol) {
        }
        URL(const URL& obj) = default;
        URL(URL&& obj) = default;

        std::string build() const;

        std::string host;
        std::string path;
        Protocol protocol;
        std::pmr::map<std::string, std::string> query_params;

      private:
        std::string _build_params() const;
    };

} // namespace TAOW::http_client