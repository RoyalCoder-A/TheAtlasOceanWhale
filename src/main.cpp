#include "taow/http_client.hpp"
#include "taow/url.hpp"
#include <iostream>
int main() {
    std::cout << "Starting process..." << std::endl;
    TAOW::http_client::Client client{TAOW::http_client::URL{TAOW::http_client::URLSchema::https, "api.myip.com", "/"},
                                     TAOW::http_client::HttpMethod::GET};
    auto result = client.call();
    std::cout << result.body_text() << std::endl;
    return 0;
}
