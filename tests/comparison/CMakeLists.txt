cmake_minimum_required(VERSION 3.11)
project("EFFICIENCE_COMPARISON")

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(data data_for_bar_chart.cpp)
target_link_libraries(data ${GTEST_LIBRARIES} gtest_main pthread)