#include "taow/json.hpp"
#include "catch2/catch_test_macros.hpp"
#include <string>

TEST_CASE("Test de serialization") {
    SECTION("Test normal deserialization") {
#define TEST_STRUCT_DEFINITION(X)                                                                                      \
    X(std::string, name, SCALAR, REQUIRED)                                                                             \
    X(int, age, SCALAR, REQUIRED)
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
#define NORMAL_LIST_DEFINITION(X) X(int, values, LIST, REQUIRED)
        CREATE_STRUCT_WITH_FROM_JSON(NormalList, NORMAL_LIST_DEFINITION);
        auto res = NormalList::from_json_str(R"(
            {
            "values": [1, 2, 3]
            }
            )");
        REQUIRE(res.values[1] == 2);
    }

    SECTION("Test nested object deserialization") {
#define NESTED_DEFINITION(X) X(TestStruct, nested, OBJ, REQUIRED)
        CREATE_STRUCT_WITH_FROM_JSON(TestStruct, TEST_STRUCT_DEFINITION);
        CREATE_STRUCT_WITH_FROM_JSON(Nested, NESTED_DEFINITION);
        auto res = Nested::from_json_str(R"(
            {
                "nested": {
                    "name": "Armin",
                    "age": 25
                }
            }
            )");
        REQUIRE(res.nested.name == "Armin");
        REQUIRE(res.nested.age == 25);
    }

    SECTION("Test list object deserialization") {
        CREATE_STRUCT_WITH_FROM_JSON(TestStruct, TEST_STRUCT_DEFINITION);
#define NESTED_OBJ_DEFINITION(X) X(TestStruct, nested, LISTOBJ, REQUIRED)
        CREATE_STRUCT_WITH_FROM_JSON(NestedObj, NESTED_OBJ_DEFINITION);
        auto res = NestedObj::from_json_str(R"(
            {
                "nested": [
                    {
                        "name": "Armin",
                        "age": 25
                    },
                    {
                        "name": "Amin",
                        "age": 23
                    }
                ]
            }
            )");
        REQUIRE(res.nested[0].name == "Armin");
        REQUIRE(res.nested[1].name == "Amin");
    }

    SECTION("Test optional field deserialization") {
        CREATE_STRUCT_WITH_FROM_JSON(TestStruct, TEST_STRUCT_DEFINITION);
#define OPTIONAL_STRUCT_DEFINITION(X)                                                                                  \
    X(std::string, name, SCALAR, REQUIRED)                                                                             \
    X(std::string, this_one, SCALAR, OPTIONAL)                                                                         \
    X(TestStruct, another_one, LISTOBJ, OPTIONAL)
        CREATE_STRUCT_WITH_FROM_JSON(OptionalStruct, OPTIONAL_STRUCT_DEFINITION);

        auto res = OptionalStruct::from_json_str(R"(
        {
            "name": "Armin",
            "another_one": [
                {"name": "Amin", "age": 23}
            ]
        }
            )");
        REQUIRE(!res.this_one);
        REQUIRE(res.another_one);
        REQUIRE(res.another_one.value()[0].name == "Amin");
        REQUIRE(res.another_one.value()[0].age == 23);
    }
}