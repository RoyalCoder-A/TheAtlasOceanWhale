#include "taow/http_client.hpp"
#include <iostream>
int main() {
    TAOW::http_client::Client client{"api.restful-api.dev", "/objects", TAOW::http_client::HttpMethod::GET};
    auto res = client.call();
    std::cout << "Result: " << res.body_text() << std::endl;
    return 0;
}