cmake_minimum_required(VERSION 3.20)
project(ch12_testing_debugging LANGUAGES CXX)

include(CTest)

add_library(ch12_project_options INTERFACE)
target_compile_features(ch12_project_options INTERFACE cxx_std_20)

if(MSVC)
  target_compile_options(ch12_project_options INTERFACE /W4 /permissive-)
else()
  target_compile_options(ch12_project_options INTERFACE
    -Wall -Wextra -Wpedantic)
endif()

add_library(ch12_portfolio STATIC src/portfolio.cpp)
target_include_directories(ch12_portfolio PUBLIC include)
target_link_libraries(ch12_portfolio PUBLIC ch12_project_options)

add_executable(ch12_portfolio_tests tests/test_portfolio.cpp)
target_link_libraries(ch12_portfolio_tests PRIVATE ch12_portfolio)
add_test(NAME ch12_execution_portfolio_fee_behavior
  COMMAND ch12_portfolio_tests)

add_executable(ch12_portfolio_overflow_tests
  tests/test_portfolio_overflow.cpp)
target_link_libraries(ch12_portfolio_overflow_tests PRIVATE ch12_portfolio)
add_test(NAME ch12_position_quantity_overflow_is_rejected
  COMMAND ch12_portfolio_overflow_tests)

add_executable(ch12_logic_fix_solution exercises/logic_fix_solution.cpp)
target_link_libraries(ch12_logic_fix_solution PRIVATE ch12_project_options)
add_test(NAME ch12_logic_fix_solution_runs COMMAND ch12_logic_fix_solution)
set_tests_properties(ch12_logic_fix_solution_runs PROPERTIES
  PASS_REGULAR_EXPRESSION "fixed-cash=799")

# These programs intentionally contain defects.  Declaring them keeps every
# complete listing build-addressable without putting a failure demo in `all`.
add_executable(ch12_assert_only_release EXCLUDE_FROM_ALL
  failures/assert_only_test.cpp)
add_executable(ch12_dangling_pointer_failure EXCLUDE_FROM_ALL
  failures/dangling_pointer.cpp)
add_executable(ch12_double_delete_failure EXCLUDE_FROM_ALL
  failures/double_delete.cpp)
add_executable(ch12_logic_bug_failure EXCLUDE_FROM_ALL
  failures/logic_bug.cpp)
add_executable(ch12_out_of_bounds_failure EXCLUDE_FROM_ALL
  failures/out_of_bounds.cpp)
add_executable(ch12_signed_overflow_failure EXCLUDE_FROM_ALL
  failures/signed_overflow.cpp)
foreach(failure_target IN ITEMS
    ch12_assert_only_release
    ch12_dangling_pointer_failure
    ch12_double_delete_failure
    ch12_logic_bug_failure
    ch12_out_of_bounds_failure
    ch12_signed_overflow_failure)
  target_link_libraries(${failure_target} PRIVATE ch12_project_options)
endforeach()

if(QUANT_BUILD_FUZZERS)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "QUANT_BUILD_FUZZERS requires Clang with libFuzzer")
  endif()
  add_executable(quant_csv_fuzzer fuzz/csv_parser_fuzz.cpp)
  target_include_directories(quant_csv_fuzzer PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../project/include)
  target_link_libraries(quant_csv_fuzzer PRIVATE ch12_project_options)
  target_compile_options(quant_csv_fuzzer PRIVATE -fsanitize=fuzzer,address)
  target_link_options(quant_csv_fuzzer PRIVATE -fsanitize=fuzzer,address)
endif()
