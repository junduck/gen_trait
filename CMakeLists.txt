cmake_minimum_required(VERSION 3.10)
project(gen_trait_sample)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(gtest/googletest)
add_executable(gen_trait_sample src/main.cpp src/callable.cpp src/drawable.cpp)
target_include_directories(gen_trait_sample PRIVATE sample)
target_link_libraries(gen_trait_sample gtest gtest_main)
