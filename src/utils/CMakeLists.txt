cmake_minimum_required(VERSION 3.16)
add_library(taow_utils string_utils.cpp date_time_utils.cpp)
add_library(TAOW::utils ALIAS taow_utils)

target_include_directories(
    taow_utils PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

install(TARGETS taow_utils DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)
