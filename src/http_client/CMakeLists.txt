cmake_minimum_required(VERSION 3.16)

add_library(http_client STATIC url.cpp)
add_library(TAOW::http_client ALIAS http_client)

target_include_directories(
    http_client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

find_package(Boost CONFIG REQUIRED)

target_link_libraries(http_client PUBLIC Boost::headers TAOW::utility)