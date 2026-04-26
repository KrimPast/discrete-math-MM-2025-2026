cmake_minimum_required(VERSION 4.2)
find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

project(discrete_math_MM_2025_2026)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-fopenmp)
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
