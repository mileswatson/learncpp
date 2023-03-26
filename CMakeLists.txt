cmake_minimum_required(VERSION 3.19)
project(LearnCPP VERSION 1.0.0)

add_library(
    learncpp STATIC
    src/learncpp/square/square.hpp
    src/learncpp/square/square.cpp
)
target_include_directories(learncpp PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(main apps/main.cpp)
target_link_libraries(main PRIVATE learncpp)
