cmake_minimum_required(VERSION 3.19)
project(LearnCPP VERSION 1.0.0)

set (CMAKE_CXX_STANDARD 20)

add_library(
    learncpp STATIC
    src/learncpp/automata/dfa.hpp
    src/learncpp/automata/dfa_node.hpp
    src/learncpp/automata/nfa.hpp
    src/learncpp/automata/nfa_node.hpp
    src/learncpp/square/square.hpp
    src/learncpp/square/square.cpp
)
target_include_directories(learncpp PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(main apps/main.cpp)
target_link_libraries(main PRIVATE learncpp)

add_subdirectory(tests)
