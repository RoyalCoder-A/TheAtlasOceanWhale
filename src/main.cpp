#include "taow/form_encoding.hpp"
#include "taow/http_client.hpp"
#include "taow/url.hpp"
#include <iostream>
int main() {
    TAOW::http_client::Client client{
        TAOW::http_client::FormRequest{{"x", {"1"}}, {"y", {{"1"}, {"2"}, {"3"}}}, {"z", {{"a", {"1"}}, {"b", {"2"}}}}},
        TAOW::http_client::URL{"webhook.site", "/cbeab4f7-7376-44d9-b3ef-8ca1aa628dab", {{"x", {"1"}}}},
        TAOW::http_client::HttpMethod::POST};
    auto res = client.call();
    const auto x = res.body_text();
    std::cout << "Result: " << x << std::endl;
    return 0;
}