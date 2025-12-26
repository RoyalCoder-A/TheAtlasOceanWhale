#include "taow/json.hpp"
#include "catch2/catch_test_macros.hpp"
#include <string>

#define TEST_STRUCT_DEFINITION(X)                                                                                      \
    X(std::string, name, SCALAR, REQUIRED)                                                                             \
    X(int, age, SCALAR, REQUIRED)

TEST_CASE("Test de serialization") {
    SECTION("Test normal deserialization") {
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
#undef NORMAL_LIST_DEFINITION
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
#undef NESTED_DEFINITION
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
#undef NESTED_OBJ_DEFINITION
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
#undef OPTIONAL_STRUCT_DEFINITION
    }
}

TEST_CASE("Test serialization") {
    SECTION("Test normal serialization") {
        CREATE_STRUCT_WITH_TO_JSON(TestStruct, TEST_STRUCT_DEFINITION);
        TestStruct obj{.name = "Armin", .age = 25};
        auto res = obj.to_json_str();
        REQUIRE(res == R"({"age":25,"name":"Armin"})");
    }

    SECTION("Test normal list serialization") {
#define NORMAL_LIST_DEFINITIONS(X) X(int, values, LIST, REQUIRED)
        CREATE_STRUCT_WITH_TO_JSON(NormalList, NORMAL_LIST_DEFINITIONS);
        NormalList obj{.values = {1, 2, 3}};
        auto res = obj.to_json_str();
        REQUIRE(res == R"({"values":[1,2,3]})");
#undef NORMAL_LIST_DEFINITIONS
    }

    SECTION("Test nested object serialization") {
#define NESTED_DEFINITIONS(X) X(TestStruct, nested, OBJ, REQUIRED)
        CREATE_STRUCT_WITH_TO_JSON(TestStruct, TEST_STRUCT_DEFINITION);
        CREATE_STRUCT_WITH_TO_JSON(Nested, NESTED_DEFINITIONS);
        Nested obj{.nested = TestStruct{.name = "Armin", .age = 25}};
        auto res = obj.to_json_str();
        REQUIRE(res == R"({"nested":{"age":25,"name":"Armin"}})");
#undef NESTED_DEFINITIONS
    }

    SECTION("Test nested array serialization") {
#define NESTED_ARRAY_DEFINITIONS(X) X(TestStruct, nested, LISTOBJ, REQUIRED)
        CREATE_STRUCT_WITH_TO_JSON(TestStruct, TEST_STRUCT_DEFINITION);
        CREATE_STRUCT_WITH_TO_JSON(NestedArray, NESTED_ARRAY_DEFINITIONS);
        NestedArray obj{.nested = {{.name = "Armin", .age = 25}}};
        auto res = obj.to_json_str();
        REQUIRE(res == R"({"nested":[{"age":25,"name":"Armin"}]})");
#undef NESTED_ARRAY_DEFINITIONS
    }

    SECTION("Test optional serialization") {
#define OPTIONAL_DEFINITIONS(X)                                                                                        \
    X(std::string, name, SCALAR, REQUIRED)                                                                             \
    X(int, values, LIST, OPTIONAL)                                                                                     \
    X(TestStruct, nested, LISTOBJ, OPTIONAL)
        CREATE_STRUCT_WITH_TO_JSON(TestStruct, TEST_STRUCT_DEFINITION);
        CREATE_STRUCT_WITH_TO_JSON(OptionalStruct, OPTIONAL_DEFINITIONS);
        OptionalStruct obj{.name = "Armin", .values = {{1, 2, 3}}, .nested = {{{.name = "Amin", .age = 23}}}};
        auto res = obj.to_json_str();
        REQUIRE(res == R"({"name":"Armin","nested":[{"age":23,"name":"Amin"}],"values":[1,2,3]})");
#undef OPTIONAL_STRUCT_DEFINITION
    }
}