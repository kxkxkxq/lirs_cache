cmake_minimum_required(VERSION 3.11)
project(src)

set(INCLUDE_DIR  "${CMAKE_SOURCE_DIR}/include")

add_executable(lirs lirs.cpp)
target_include_directories(lirs PRIVATE ${INCLUDE_DIR})

add_executable(belady belady.cpp)
target_include_directories(belady PRIVATE ${INCLUDE_DIR})