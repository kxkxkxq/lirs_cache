cmake_minimum_required(VERSION 3.11)
project(tests)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(INCLUDE_DIR  "${CMAKE_SOURCE_DIR}/include")

add_executable(tests test.cpp)
target_include_directories(tests PRIVATE ${INCLUDE_DIR})

target_link_libraries(tests ${GTEST_LIBRARIES} gtest_main pthread)