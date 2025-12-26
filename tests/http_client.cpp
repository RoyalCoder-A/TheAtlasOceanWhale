#include "taow/http_client.hpp"
#include "catch2/catch_test_macros.hpp"

using TAOW::http_client::Client;
using TAOW::http_client::HttpMethod;

TEST_CASE("Http client json test") {
    SECTION("Simple get request") {
        Client client{"api.restful-api.dev", "/objects", HttpMethod::GET};
        auto res = client.call();
        REQUIRE(res.text() == "Some placeholder!");
    }
}