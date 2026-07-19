cmake_minimum_required(VERSION 3.20)
project(ch09_constrained_strategies LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

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
