cmake_minimum_required(VERSION 3.16)
add_library(taow_utils STATIC string_utils.cpp)
add_library(TAOW::utils ALIAS taow_utils)

target_include_directories(taow_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)