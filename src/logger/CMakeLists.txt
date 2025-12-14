cmake_minimum_required(VERSION 3.16.0)
add_library(logger STATIC main.cpp)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

target_include_directories(logger PUBLIC include)

Include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1

FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG        0.8.0
)

FetchContent_MakeAvailable(fmt yaml-cpp)

target_link_libraries(logger PUBLIC fmt::fmt yaml-cpp::yaml-cpp)