cmake_minimum_required(VERSION 3.16)

add_library(taow_http_client STATIC http_client.cpp result.cpp)
add_library(TAOW::http_client ALIAS taow_http_client)
target_include_directories(taow_http_client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)


find_package(Boost CONFIG REQUIRED)
target_include_directories(taow_http_client PRIVATE ${Boost_INCLUDE_DIRS})

find_package(ZLIB REQUIRED)
target_link_libraries(taow_http_client PRIVATE ZLIB::ZLIB)

target_link_libraries(taow_http_client PUBLIC TAOW::utils)