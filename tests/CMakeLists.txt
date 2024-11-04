cmake_minimum_required(VERSION 3.11)
project("HWC_tests")

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(tests tests.cpp)
target_link_libraries(tests ${GTEST_LIBRARIES} gtest_main pthread)