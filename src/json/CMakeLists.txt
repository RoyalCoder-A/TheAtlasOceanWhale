cmake_minimum_required(VERSION 3.16)
add_library(taow_json INTERFACE)
add_library(TAOW::json ALIAS taow_json)

target_include_directories(
    taow_json INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

find_package(nlohmann_json REQUIRED)

target_link_libraries(taow_json INTERFACE nlohmann_json::nlohmann_json)

install(TARGETS taow_json)
install(DIRECTORY include/ DESTINATION include)
