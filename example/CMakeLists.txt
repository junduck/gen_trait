cmake_minimum_required(VERSION 3.14)
project(gen_trait_example)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/GoogleBenchAndTest.cmake)

add_executable(gen_trait_example
  src/main.cpp
  src/callable.cpp
  src/drawable.cpp
)
target_include_directories(gen_trait_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(gen_trait_example gtest gtest_main)
