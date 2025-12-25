#include "catch2/catch_test_macros.hpp"
#include "taow/json/macros.hpp"
#include <string>

TEST_CASE("Test json de serializing") {
#define TEST_STRUCT_DEFINITION(X)                                                                                      \
    X(std::string, name, SCALAR, REQUIRED)                                                                             \
    X(int, age, SCALAR, OPTIONAL)

    CREATE_DESERIALIZABLE_STRUCT(TestStruct, TEST_STRUCT_DEFINITION);
    SECTION("Test simple deserializing") {
        auto res = TestStruct::from_json_str(R"(
            {
                "name": "Armin",
                "age": 25
            }
            )");
        REQUIRE(res.name == "Armin");
        REQUIRE(res.age == 25);
    }

    SECTION("Nested deserializing test") {
#define TEST_WRAPPER_DEFINITION(X)                                                                                     \
    X(TestStruct, test_list, LIST, REQUIRED)                                                                           \
    X(std::string, sample, SCALAR, OPTIONAL)
        CREATE_DESERIALIZABLE_STRUCT(TestWrapper, TEST_WRAPPER_DEFINITION)

        auto res = TestWrapper::from_json_str(R"({
            "test_list": [
                {"name": "Armin", "age": 25},
                {"name": "Amin"}
            ]
            
            })");
        REQUIRE(res.test_list.size() == 2);
        REQUIRE(!res.sample);
        REQUIRE(res.test_list[0].age == 25);
    }
}