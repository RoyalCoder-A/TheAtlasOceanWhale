cmake_minimum_required(VERSION 3.16)
add_library(taow_logging logging.cpp log_manager.cpp)
add_library(TAOW::logging ALIAS taow_logging)
target_include_directories(
    taow_logging PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(taow_logging PUBLIC TAOW::utils)
