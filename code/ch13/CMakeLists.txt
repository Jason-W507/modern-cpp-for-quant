cmake_minimum_required(VERSION 3.20)
project(ch13_layout_allocation LANGUAGES CXX)

include(CTest)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_library(ch13_project_options INTERFACE)
target_compile_features(ch13_project_options INTERFACE cxx_std_20)
if(MSVC)
  target_compile_options(ch13_project_options INTERFACE /W4 /permissive-)
else()
  target_compile_options(ch13_project_options INTERFACE
    -Wall -Wextra -Wpedantic)
endif()

add_executable(ch13_quote_scan app/quote_scan.cpp)
target_link_libraries(ch13_quote_scan PRIVATE ch13_project_options)

add_executable(ch13_object_layout experiments/object_layout.cpp)
target_link_libraries(ch13_object_layout PRIVATE ch13_project_options)

add_executable(ch13_aos_soa experiments/aos_soa.cpp)
target_link_libraries(ch13_aos_soa PRIVATE ch13_project_options)

add_executable(ch13_preallocation experiments/preallocation.cpp)
target_link_libraries(ch13_preallocation PRIVATE ch13_project_options)

add_executable(ch13_arena experiments/arena.cpp)
target_link_libraries(ch13_arena PRIVATE ch13_project_options)

add_executable(ch13_false_sharing_layout
  experiments/false_sharing_layout.cpp)
target_link_libraries(ch13_false_sharing_layout PRIVATE ch13_project_options)

add_executable(ch13_arena_dangling_failure EXCLUDE_FROM_ALL
  failures/arena_dangling.cpp)
target_link_libraries(ch13_arena_dangling_failure PRIVATE ch13_project_options)

add_test(NAME ch13_quote_scan_preserves_threshold_behavior
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../../tools/expect_output.py
          --expected ${CMAKE_CURRENT_LIST_DIR}/expected/quote_scan.txt
          -- $<TARGET_FILE:ch13_quote_scan>)

add_test(NAME ch13_object_layout_invariants_hold COMMAND ch13_object_layout)
set_tests_properties(ch13_object_layout_invariants_hold PROPERTIES
  PASS_REGULAR_EXPRESSION "layout-facts-ok")

add_test(NAME ch13_aos_soa_checksums_match COMMAND ch13_aos_soa)
set_tests_properties(ch13_aos_soa_checksums_match PROPERTIES
  PASS_REGULAR_EXPRESSION "aos-soa-ok rows=5 checksum=1496.00")

add_test(NAME ch13_preallocation_preserves_results COMMAND ch13_preallocation)
set_tests_properties(ch13_preallocation_preserves_results PROPERTIES
  PASS_REGULAR_EXPRESSION "preallocation-ok rows=32 checksum=3696.00")

add_test(NAME ch13_arena_preserves_results COMMAND ch13_arena)
set_tests_properties(ch13_arena_preserves_results PROPERTIES
  PASS_REGULAR_EXPRESSION "arena-ok rows=3 checksum=608.00 buffer-bytes=4096")

add_test(NAME ch13_false_sharing_assumption_is_labeled
  COMMAND ch13_false_sharing_layout)
set_tests_properties(ch13_false_sharing_assumption_is_labeled PROPERTIES
  PASS_REGULAR_EXPRESSION
    "false-sharing-layout-ok assumed-line-bytes=64")
