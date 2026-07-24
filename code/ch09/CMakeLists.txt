cmake_minimum_required(VERSION 3.20)
project(ch09_constrained_strategies LANGUAGES CXX)

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

function(ch09_options target)
  target_sources(${target} PRIVATE src/domain.cpp)
  target_include_directories(${target} PRIVATE include tests)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4 /permissive-)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endfunction()

add_executable(ch09_test_static_strategy
  tests/test_static_strategy.cpp tests/test_support.cpp)
ch09_options(ch09_test_static_strategy)
add_test(NAME ch09_static_strategy_decision_seam
  COMMAND ch09_test_static_strategy)

add_executable(ch09_ordinary_strategy ordinary_strategy.cpp)
ch09_options(ch09_ordinary_strategy)
add_executable(ch09_function_template function_template.cpp)
ch09_options(ch09_function_template)
add_executable(ch09_constrained_pipeline src/main.cpp)
ch09_options(ch09_constrained_pipeline)
add_executable(ch09_polymorphism_compare polymorphism_compare.cpp)
ch09_options(ch09_polymorphism_compare)
add_executable(ch09_maximum_position_solution
  exercises/maximum_position_solution.cpp)
ch09_options(ch09_maximum_position_solution)

add_test(NAME ch09_ordinary_strategy_runs COMMAND ch09_ordinary_strategy)
add_test(NAME ch09_function_template_runs COMMAND ch09_function_template)
add_test(NAME ch09_constrained_pipeline_runs COMMAND ch09_constrained_pipeline)
add_test(NAME ch09_polymorphism_compare_runs COMMAND ch09_polymorphism_compare)
add_test(NAME ch09_maximum_position_solution_runs
  COMMAND ch09_maximum_position_solution)
if(QUANT_BUILD_NEGATIVE_TESTS)
  add_test(NAME ch09_unconstrained_error_exposes_missing_member
    COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_LIST_DIR}/../../tools/expect_compile_failure.py
            --compiler ${CMAKE_CXX_COMPILER}
            --source ${CMAKE_CURRENT_LIST_DIR}/failures/unconstrained_strategy.cpp
            --output ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch09_unconstrained.obj
            --diagnostic "request for member.*on_event|has no member.*on_event|member reference base type.*is not a structure or union|left of.*on_event.*must have class/struct/union")
  add_test(NAME ch09_concept_error_reports_unsatisfied_constraint
    COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_LIST_DIR}/../../tools/expect_compile_failure.py
            --compiler ${CMAKE_CXX_COMPILER}
            --source ${CMAKE_CURRENT_LIST_DIR}/failures/constrained_strategy.cpp
            --output ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch09_constrained.obj
            --diagnostic "constraints not satisfied|constraint.*not satisfied")
endif()
add_test(NAME ch09_clean_constrained_pipeline_configures_builds_and_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../../tools/test_clean_cmake_project.py
          --cmake ${CMAKE_COMMAND}
          --source ${CMAKE_CURRENT_LIST_DIR}
          --expected ${CMAKE_CURRENT_LIST_DIR}/expected/constrained_pipeline.txt
          --target ch09_constrained_pipeline
          --generator "${CMAKE_GENERATOR}"
          --compiler ${CMAKE_CXX_COMPILER}
          --work-root ${CMAKE_CURRENT_BINARY_DIR}/clean-build-tests
          --config $<CONFIG>)
