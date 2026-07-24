cmake_minimum_required(VERSION 3.20)
project(ch10_error_observability LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
if(BUILD_TESTING)
  find_package(Python3 REQUIRED COMPONENTS Interpreter)
endif()

function(ch10_options target)
  target_include_directories(${target} PRIVATE include tests)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4 /permissive-)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endfunction()

function(ch10_core target)
  target_sources(${target} PRIVATE
    src/csv_reader.cpp src/domain.cpp src/pipeline.cpp src/portfolio.cpp)
  ch10_options(${target})
endfunction()

add_executable(ch10_test_atomic_portfolio
  tests/test_atomic_portfolio.cpp tests/test_support.cpp)
ch10_core(ch10_test_atomic_portfolio)
add_test(NAME ch10_execution_portfolio_strong_guarantee_seam
  COMMAND ch10_test_atomic_portfolio)

add_executable(ch10_test_csv_errors
  tests/test_csv_errors.cpp tests/test_support.cpp)
ch10_core(ch10_test_csv_errors)
add_test(NAME ch10_market_input_preserves_error_context_seam
  COMMAND ch10_test_csv_errors)

add_executable(ch10_test_observability
  tests/test_observability.cpp tests/test_support.cpp)
ch10_core(ch10_test_observability)
add_test(NAME ch10_boundary_logs_error_once_seam
  COMMAND ch10_test_observability)

add_executable(ch10_recovery_pipeline src/main.cpp)
ch10_core(ch10_recovery_pipeline)

add_executable(ch10_error_choices error_choices.cpp)
target_sources(ch10_error_choices PRIVATE src/csv_reader.cpp src/domain.cpp)
ch10_options(ch10_error_choices)

add_executable(ch10_error_injection_solution
  exercises/error_injection_solution.cpp)
target_sources(ch10_error_injection_solution PRIVATE
  src/domain.cpp src/portfolio.cpp)
ch10_options(ch10_error_injection_solution)

add_test(NAME ch10_recovery_pipeline_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../../tools/expect_process.py
          --expected-exit 0
          --expected-stdout ${CMAKE_CURRENT_LIST_DIR}/expected/recovery_pipeline.txt
          --expected-stderr ${CMAKE_CURRENT_LIST_DIR}/expected/recovery_pipeline_errors.txt
          -- $<TARGET_FILE:ch10_recovery_pipeline>
             ${CMAKE_CURRENT_LIST_DIR}/data/fills.csv)
add_test(NAME ch10_external_failure_is_logged_once
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../../tools/expect_process.py
          --expected-exit 2
          --expected-stderr ${CMAKE_CURRENT_LIST_DIR}/expected/missing_file_error.txt
          -- $<TARGET_FILE:ch10_recovery_pipeline> missing.csv)
add_test(NAME ch10_error_choices_run
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../../tools/expect_output.py
          --expected ${CMAKE_CURRENT_LIST_DIR}/expected/error_choices.txt
          -- $<TARGET_FILE:ch10_error_choices>)
add_test(NAME ch10_error_injection_solution_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../../tools/expect_output.py
          --expected ${CMAKE_CURRENT_LIST_DIR}/expected/error_injection_solution.txt
          -- $<TARGET_FILE:ch10_error_injection_solution>)
add_test(NAME ch10_clean_recovery_pipeline_configures_builds_and_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../../tools/test_clean_cmake_project.py
          --cmake ${CMAKE_COMMAND}
          --source ${CMAKE_CURRENT_LIST_DIR}
          --expected ${CMAKE_CURRENT_LIST_DIR}/expected/recovery_pipeline.txt
          --expected-stderr ${CMAKE_CURRENT_LIST_DIR}/expected/recovery_pipeline_errors.txt
          --target ch10_recovery_pipeline
          --argument ${CMAKE_CURRENT_LIST_DIR}/data/fills.csv
          --generator "${CMAKE_GENERATOR}"
          --compiler ${CMAKE_CXX_COMPILER}
          --work-root ${CMAKE_CURRENT_BINARY_DIR}/clean-build-tests
          --config $<CONFIG>)
