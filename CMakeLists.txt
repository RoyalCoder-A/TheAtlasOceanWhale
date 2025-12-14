cmake_minimum_required(VERSION 3.16)
project(TAOW CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSION OFF)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
message(IPO\ Supported?\  ${ipo_supported})
set(INTERPROCEDURAL_OPTIMIZATION ${ipo_supported})

enable_testing()
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/test)