#include "taow/http_client.hpp"
#include <iostream>
int main() {
    TAOW::http_client::Client client{"api.restful-api.dev", "/objects", TAOW::http_client::HttpMethod::GET};
    auto res = client.call();
    const auto x = res.body_text();
    std::cout << "Result: " << x << std::endl;
    return 0;
}