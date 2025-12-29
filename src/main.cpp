#include "taow/form_encoding.hpp"
#include "taow/http_client.hpp"
#include "taow/multipart.hpp"
#include "taow/url.hpp"
#include <cstdint>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>
int main() {
    std::ifstream file{"/home/armin/develop/TheAtlasOceanWhale/CMakeLists.txt", std::ios_base::binary};
    if (!file)
        throw std::runtime_error("Can't open file!");
    std::vector<std::uint8_t> bytes{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

    TAOW::http_client::Client client{
        TAOW::http_client::Multipart{{{"file", TAOW::http_client::FileUpload{"test.txt", "text/plain", bytes}}},
                                     {
                                         {"x", "y"},
                                         {"name", "armin"},
                                     }},
        TAOW::http_client::URL{"webhook.site", "/cbeab4f7-7376-44d9-b3ef-8ca1aa628dab", {{"x", {"1"}}}},
        TAOW::http_client::HttpMethod::POST};
    auto res = client.call();
    const auto x = res.body_text();
    std::cout << "Result: " << x << std::endl;
    return 0;
}