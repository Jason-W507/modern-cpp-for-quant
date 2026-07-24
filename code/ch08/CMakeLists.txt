cmake_minimum_required(VERSION 3.20)
project(ch08_domain_components LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
if(BUILD_TESTING)
  find_package(Python3 REQUIRED COMPONENTS Interpreter)
endif()
if(NOT DEFINED QUANT_BUILD_NEGATIVE_TESTS)
  set(QUANT_BUILD_NEGATIVE_TESTS ON)
endif()

function(ch08_target_options target)
  target_include_directories(${target} PRIVATE include tests)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4 /permissive-)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endfunction()

function(ch08_target target)
  add_executable(${target} ${ARGN} src/domain.cpp)
  ch08_target_options(${target})
endfunction()

ch08_target(ch08_test_strategy tests/test_strategy.cpp)
target_sources(ch08_test_strategy PRIVATE tests/test_support.cpp)
add_test(NAME ch08_strategy_decision_seam COMMAND ch08_test_strategy)

ch08_target(ch08_test_execution_portfolio tests/test_execution_portfolio.cpp)
target_sources(ch08_test_execution_portfolio PRIVATE tests/test_support.cpp)
add_test(NAME ch08_execution_portfolio_seam
  COMMAND ch08_test_execution_portfolio)

add_executable(ch08_domain_pipeline
  src/main.cpp src/domain.cpp src/event_reader.cpp)
ch08_target_options(ch08_domain_pipeline)
ch08_target(ch08_runtime_strategy runtime_strategy.cpp)
ch08_target(ch08_test_invalid_values tests/test_invalid_values.cpp)
target_sources(ch08_test_invalid_values PRIVATE tests/test_support.cpp)
ch08_target(ch08_sell_down_solution exercises/sell_down_solution.cpp)

add_test(NAME ch08_domain_pipeline_runs
  COMMAND ch08_domain_pipeline ${CMAKE_CURRENT_SOURCE_DIR}/data/events.csv)
add_test(NAME ch08_runtime_strategy_runs COMMAND ch08_runtime_strategy)
add_test(NAME ch08_invalid_values_are_rejected COMMAND ch08_test_invalid_values)
add_test(NAME ch08_sell_down_solution_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../../tools/expect_output.py
          --expected ${CMAKE_CURRENT_LIST_DIR}/expected/sell_down_solution.txt
          -- $<TARGET_FILE:ch08_sell_down_solution>)
if(QUANT_BUILD_NEGATIVE_TESTS)
  add_test(NAME ch08_wrong_override_fails_during_compilation
    COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_LIST_DIR}/../../tools/expect_compile_failure.py
            --compiler ${CMAKE_CXX_COMPILER}
            --source ${CMAKE_CURRENT_LIST_DIR}/failures/wrong_override.cpp
            --output ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch08_wrong_override.obj
            --diagnostic "does not override|did not override|marked.*override|override.*but does not")
endif()
add_test(NAME ch08_clean_domain_pipeline_configures_builds_and_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../../tools/test_clean_cmake_project.py
          --cmake ${CMAKE_COMMAND}
          --source ${CMAKE_CURRENT_LIST_DIR}
          --expected ${CMAKE_CURRENT_LIST_DIR}/expected/domain_pipeline.txt
          --target ch08_domain_pipeline
          --argument ${CMAKE_CURRENT_LIST_DIR}/data/events.csv
          --generator "${CMAKE_GENERATOR}"
          --compiler ${CMAKE_CXX_COMPILER}
          --work-root ${CMAKE_CURRENT_BINARY_DIR}/clean-build-tests
          --config $<CONFIG>)
