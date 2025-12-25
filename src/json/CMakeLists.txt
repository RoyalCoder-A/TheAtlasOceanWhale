cmake_minimum_required(VERSION 3.16)

add_library(taow_json INTERFACE)
add_library(TAOW::json ALIAS taow_json)

target_include_directories(taow_json INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)
target_link_libraries(taow_json INTERFACE nlohmann_json::nlohmann_json)