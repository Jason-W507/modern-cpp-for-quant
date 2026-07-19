cmake_minimum_required(VERSION 3.20)
project(ch14_measurement_lab LANGUAGES CXX)

include(CTest)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_library(ch14_project_options INTERFACE)
target_compile_features(ch14_project_options INTERFACE cxx_std_20)
if(MSVC)
  target_compile_options(ch14_project_options INTERFACE /W4 /permissive-)
else()
  target_compile_options(ch14_project_options INTERFACE
    -Wall -Wextra -Wpedantic)
endif()

add_executable(ch14_statistics_report experiments/statistics_report.cpp)
target_link_libraries(ch14_statistics_report PRIVATE ch14_project_options)

add_executable(ch14_layout_benchmark experiments/layout_benchmark.cpp)
target_link_libraries(ch14_layout_benchmark PRIVATE ch14_project_options)
target_compile_definitions(ch14_layout_benchmark PRIVATE
  CH14_BUILD_CONFIGURATION="$<CONFIG>")

add_executable(ch14_amdahl_report experiments/amdahl_report.cpp)
target_link_libraries(ch14_amdahl_report PRIVATE ch14_project_options)

add_executable(ch14_profile_workload experiments/profile_workload.cpp)
target_link_libraries(ch14_profile_workload PRIVATE ch14_project_options)

add_executable(ch14_checksum_mismatch_failure EXCLUDE_FROM_ALL
  failures/checksum_mismatch.cpp)
target_link_libraries(ch14_checksum_mismatch_failure
  PRIVATE ch14_project_options)

add_test(NAME ch14_statistics_report_explains_distribution_shape
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../../tools/expect_output.py
          --expected ${CMAKE_CURRENT_LIST_DIR}/expected/statistics_report.txt
          -- $<TARGET_FILE:ch14_statistics_report>)

add_test(NAME ch14_layout_benchmark_preserves_business_result
  COMMAND ch14_layout_benchmark)
set_tests_properties(ch14_layout_benchmark_preserves_business_result PROPERTIES
  PASS_REGULAR_EXPRESSION
    "benchmark-ok rows=1000000 samples=21 warmups=3 alternating=true checksum-match=true")

add_test(NAME ch14_amdahl_report_respects_end_to_end_budget
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../../tools/expect_output.py
          --expected ${CMAKE_CURRENT_LIST_DIR}/expected/amdahl_report.txt
          -- $<TARGET_FILE:ch14_amdahl_report>)

add_test(NAME ch14_profile_workload_consumes_a_valid_result
  COMMAND ch14_profile_workload)
set_tests_properties(ch14_profile_workload_consumes_a_valid_result PROPERTIES
  PASS_REGULAR_EXPRESSION
    "profile-workload-ok rows=200000 checksum-finite=true")
