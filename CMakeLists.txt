cmake_minimum_required(VERSION 3.16)
project(TAOW CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_support)
message(If\ IPO\ Available?\  ${ipo_support})
set(INTERPROCEDURAL_OPTIMIZATION ${ipo_support})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

