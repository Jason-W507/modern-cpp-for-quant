cmake_minimum_required(VERSION 3.20)
project(modern_cpp_for_quant VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

function(quant_target target source)
  add_executable(${target} ${source})
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4 /permissive-)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endfunction()

function(quant_example target source)
  quant_target(${target} ${source})
endfunction()

function(quant_project_target target source)
  quant_target(${target} ${source})
  target_include_directories(${target} PRIVATE project/include project/tests)
endfunction()

quant_example(ch01_compilation_model code/ch01/compilation_model.cpp)
quant_project_target(ch02_value_semantics code/ch02/value_semantics.cpp)
quant_example(ch03_raii_file code/ch03/raii_file.cpp)
quant_project_target(ch04_ranges_pipeline code/ch04/ranges_pipeline.cpp)

quant_project_target(test_strategy project/tests/test_strategy.cpp)
add_test(NAME strategy_decision_seam COMMAND test_strategy)

quant_project_target(test_execution_portfolio project/tests/test_execution_portfolio.cpp)
add_test(NAME execution_portfolio_seam COMMAND test_execution_portfolio)

quant_project_target(test_csv_reader project/tests/test_csv_reader.cpp)
add_test(NAME market_data_input_seam COMMAND test_csv_reader)

quant_project_target(test_backtest project/tests/test_backtest.cpp)
add_test(NAME end_to_end_backtest_seam COMMAND test_backtest)

quant_project_target(quant_backtest project/src/main.cpp)
add_test(NAME sample_backtest_runs
  COMMAND quant_backtest ${CMAKE_CURRENT_SOURCE_DIR}/project/data/sample.csv)
set_tests_properties(sample_backtest_runs PROPERTIES
  PASS_REGULAR_EXPRESSION "fills=1 equity=10100 return=1%")

quant_example(layout_benchmark project/benchmarks/layout_benchmark.cpp)
quant_example(ch11_parallel_sum code/ch11/parallel_sum.cpp)
quant_example(ch12_numerical_stability code/ch12/numerical_stability.cpp)
quant_example(ch12_pybind_boundary code/ch12/pybind11_boundary.cpp)
add_test(NAME layout_benchmark_smoke COMMAND layout_benchmark)
set_tests_properties(layout_benchmark_smoke PROPERTIES
  PASS_REGULAR_EXPRESSION "checksum-match=true")
add_test(NAME ch11_parallel_sum_runs COMMAND ch11_parallel_sum)
set_tests_properties(ch11_parallel_sum_runs PROPERTIES
  PASS_REGULAR_EXPRESSION "sum=5000050000")
add_test(NAME ch12_numerical_stability_runs COMMAND ch12_numerical_stability)
set_tests_properties(ch12_numerical_stability_runs PROPERTIES
  PASS_REGULAR_EXPRESSION "naive=0 compensated=1")
add_test(NAME ch12_pybind_boundary_runs COMMAND ch12_pybind_boundary)
set_tests_properties(ch12_pybind_boundary_runs PROPERTIES
  PASS_REGULAR_EXPRESSION "pybind-boundary final-equity=103")

find_package(pybind11 CONFIG QUIET)
if(pybind11_FOUND)
  pybind11_add_module(quant_boundary MODULE code/ch12/pybind11_boundary.cpp)
  target_compile_definitions(quant_boundary PRIVATE QUANT_WITH_PYBIND11=1)
endif()

add_test(NAME ch01_compilation_model_runs COMMAND ch01_compilation_model)
set_tests_properties(ch01_compilation_model_runs PROPERTIES
  PASS_REGULAR_EXPRESSION "notional=125173.50")
add_test(NAME ch02_value_semantics_runs COMMAND ch02_value_semantics)
set_tests_properties(ch02_value_semantics_runs PROPERTIES
  PASS_REGULAR_EXPRESSION "AAPL qty=120 notional=22620")
add_test(NAME ch03_raii_file_runs COMMAND ch03_raii_file)
set_tests_properties(ch03_raii_file_runs PROPERTIES
  PASS_REGULAR_EXPRESSION "rows=2")
add_test(NAME ch04_ranges_pipeline_runs COMMAND ch04_ranges_pipeline)
set_tests_properties(ch04_ranges_pipeline_runs PROPERTIES
  PASS_REGULAR_EXPRESSION "positive-notional=3975")
