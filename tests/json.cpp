#include "taow/json.hpp"
#include "catch2/catch_test_macros.hpp"
#include <string>

TEST_CASE("Test de serialization") {
    SECTION("Test normal deserialization") {
#define TEST_STRUCT_DEFINITION(X)                                                                                      \
    X(std::string, name, SCALAR)                                                                                       \
    X(int, age, SCALAR)
        CREATE_STRUCT_WITH_FROM_JSON(TestStruct, TEST_STRUCT_DEFINITION);

        const auto res = TestStruct::from_json_str(R"(
            {
                "name": "Armin",
                "age": 25
            }
            )");
        REQUIRE(res.name == "Armin");
        REQUIRE(res.age == 25);
    }

    SECTION("Test normal list deserialization") {
#define NORMAL_LIST_DEFINITION(X) X(int, values, LIST)
        CREATE_STRUCT_WITH_FROM_JSON(NormalList, NORMAL_LIST_DEFINITION);
        auto res = NormalList::from_json_str(R"(
            {
            "values": [1, 2, 3]
            }
            )");
        REQUIRE(res.values[1] == 2);
    }
}