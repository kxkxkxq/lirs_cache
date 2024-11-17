cmake_minimum_required(VERSION 3.11)
project("HWC")

set(INCLUDE_DIR  "${CMAKE_SOURCE_DIR}/include")

add_executable(lirs sourse/lirs.cpp)
target_include_directories(lirs PUBLIC ${INCLUDE_DIR})

add_executable(belady sourse/belady.cpp)
target_include_directories(belady PUBLIC ${INCLUDE_DIR})