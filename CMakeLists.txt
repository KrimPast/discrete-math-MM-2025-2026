cmake_minimum_required(VERSION 4.2)
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

project(discrete_math_MM_2025_2026)

set(CMAKE_CXX_STANDARD 26)
add_compile_options(-fopenmp -O2)
add_link_options(-fopenmp)

add_executable(discrete_math_MM_2025_2026 main.cpp
        src/graph.cpp
        src/graph.h
        src/parsers/parser.cpp
        src/parsers/parser.h
        src/general.h
        src/analyzer.cpp
        src/analyzer.h
        src/other/other.cpp
        src/other/other.h
        tests/tests_analyzer.cpp
        tests/tests_graph.cpp
        tests/tests.h
        src/summarizer/summarizer.cpp
        src/summarizer/summarizer.h
)
