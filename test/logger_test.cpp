#include "catch2/catch_test_macros.hpp"

TEST_CASE("TMP") {
    SECTION("1 + 2 = 3") { REQUIRE(1 + 2 == 3); }
}