cmake_minimum_required(VERSION 3.20)
project(hexdump-lite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

add_library(hexdumplib 
    src/hexfmt.cpp
    src/args.cpp
)

target_include_directories(hexdumplib PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(hexdump-lite src/main.cpp)

target_link_libraries(hexdump-lite PRIVATE hexdumplib)