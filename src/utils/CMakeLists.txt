cmake_minimum_required(VERSION 3.16)
add_library(taow_utils INTERFACE)
add_library(TAOW::utils ALIAS taow_utils)

target_include_directories(taow_utils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)