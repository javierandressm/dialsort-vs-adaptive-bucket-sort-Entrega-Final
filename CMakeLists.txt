cmake_minimum_required(VERSION 3.12)

project(sort_benchmark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(sort_benchmark
    src/main.cpp
    src/benchmark.cpp
    src/DialSort.cpp
    src/AdaptiveRangeBucketSort.cpp
)

target_include_directories(sort_benchmark PRIVATE src)

if (MSVC)
    target_compile_options(sort_benchmark PRIVATE /W4)
else()
    target_compile_options(sort_benchmark PRIVATE -Wall -Wextra -pedantic)
endif()
