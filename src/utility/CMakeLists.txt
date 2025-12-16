cmake_minimum_required(VERSION 3.16)

add_library(utility INTERFACE)
add_library(TAOW::utility ALIAS utility)

target_include_directories(utility INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)