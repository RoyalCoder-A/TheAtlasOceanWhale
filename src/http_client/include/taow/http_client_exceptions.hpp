#pragma once

#include <stdexcept>
#include <string>
#include <utility>
namespace TAOW::http_client {

struct HttpClientError : std::runtime_error {
    HttpClientError(std::string message) : std::runtime_error(std::move(message)) {}
};

struct FormParsingError : HttpClientError {
    FormParsingError(std::string message) : HttpClientError(std::move(message)) {}
};

struct HttpError : HttpClientError {
    HttpError(std::string message, int status_code) : HttpClientError(message), status_code(status_code) {}
    int status_code;
};

struct BrokenResponseError : HttpClientError {
    BrokenResponseError(std::string message) : HttpClientError(message) {}
};

struct BrokenChunkError : HttpClientError {
    BrokenChunkError(std::string message) : HttpClientError(message) {}
};

struct DecompressError : HttpClientError {
    DecompressError(std::string message) : HttpClientError(message) {}
};

} // namespace TAOW::http_client
