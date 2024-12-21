cmake_minimum_required(VERSION 3.11)
project(BinSearchTree)
include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

SET(SRC_DIR src)

add_executable(Driver ${SRC_DIR}/main.cpp)
target_include_directories(Driver PUBLIC inc)

add_executable(Ref ${SRC_DIR}/ref_main.cpp)
target_include_directories(Ref PUBLIC inc)

# Benchmarking using google benchmark

find_package(benchmark REQUIRED)

add_executable(benchmark ${SRC_DIR}/benchmark.cpp)
target_include_directories(benchmark PUBLIC inc)
target_link_libraries(benchmark benchmark::benchmark)

# E2E tests
enable_testing()

SET(E2E_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/E2E_tests)

file(GLOB E2E_ALL_TESTS_FILES "${E2E_TESTS_DIR}/*.in")

foreach(file ${E2E_ALL_TESTS_FILES})
    cmake_path(GET file STEM test_stem)
    cmake_path(GET file PARENT_PATH test_parent_path)
    SET(test_name E2E_${test_stem})
    add_test(NAME ${test_name}
        COMMAND bash -c "${CMAKE_SOURCE_DIR}/scripts/runE2Etest.sh $<TARGET_FILE:Driver> ${test_parent_path}/${test_stem}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

# Unit tests with GTest
find_package(GTest REQUIRED)

add_executable(UnitTests ${SRC_DIR}/unit_tests.cpp)
target_include_directories(UnitTests PUBLIC inc)
target_link_libraries(UnitTests gtest::gtest)

include(GoogleTest)
gtest_discover_tests(UnitTests)