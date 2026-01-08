cmake_minimum_required(VERSION 3.16)
add_library(taow_cli_builder STATIC cli_builder.cpp)
add_library(TAOW::cli_builder ALIAS taow_cli_builder)

target_include_directories(taow_cli_builder PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)