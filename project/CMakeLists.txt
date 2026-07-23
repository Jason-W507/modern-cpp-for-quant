cmake_minimum_required(VERSION 3.20)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(ch17_event_driven_backtest LANGUAGES CXX)
endif()

include(CTest)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_library(ch17_project_options INTERFACE)
target_compile_features(ch17_project_options INTERFACE cxx_std_20)
if(MSVC)
  target_compile_options(ch17_project_options INTERFACE /W4 /permissive-)
else()
  target_compile_options(ch17_project_options INTERFACE
    -Wall -Wextra -Wpedantic)
endif()

function(quant_project_target target source)
  add_executable(${target} ${source})
  target_include_directories(${target} PRIVATE include tests)
  target_link_libraries(${target} PRIVATE ch17_project_options)
endfunction()

quant_project_target(quant_backtest src/main.cpp)
quant_project_target(layout_benchmark benchmarks/layout_benchmark.cpp)
quant_project_target(ch17_backtest_benchmark benchmarks/backtest_benchmark.cpp)

if(BUILD_TESTING)
  quant_project_target(test_strategy tests/test_strategy.cpp)
  quant_project_target(test_execution_portfolio tests/test_execution_portfolio.cpp)
  quant_project_target(test_csv_reader tests/test_csv_reader.cpp)
  quant_project_target(test_backtest tests/test_backtest.cpp)
  add_executable(ch17_mixed_symbol_failure EXCLUDE_FROM_ALL
    failures/mixed_symbols.cpp)
  target_include_directories(ch17_mixed_symbol_failure PRIVATE include tests)
  target_link_libraries(ch17_mixed_symbol_failure PRIVATE ch17_project_options)

  add_test(NAME strategy_decision_seam COMMAND test_strategy)
  add_test(NAME execution_portfolio_seam COMMAND test_execution_portfolio)
  add_test(NAME market_data_input_seam COMMAND test_csv_reader)
  add_test(NAME end_to_end_backtest_seam COMMAND test_backtest)
  add_test(NAME ch17_sample_backtest_prints_exact_performance_summary
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../tools/expect_output.py
          --expected ${CMAKE_CURRENT_LIST_DIR}/expected/sample_backtest.txt
          -- $<TARGET_FILE:quant_backtest>
             ${CMAKE_CURRENT_LIST_DIR}/data/sample.csv)
  add_test(NAME ch17_configured_costs_flow_to_the_cli_summary
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../tools/expect_output.py
          --expected ${CMAKE_CURRENT_LIST_DIR}/expected/sample_backtest_costs.txt
          -- $<TARGET_FILE:quant_backtest>
             ${CMAKE_CURRENT_LIST_DIR}/data/sample.csv 1.0 10.0)
  add_test(NAME ch17_invalid_execution_config_reports_one_invariant
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../tools/expect_process.py
          --expected-exit 2
          --expected-stderr
            ${CMAKE_CURRENT_LIST_DIR}/expected/invalid_config_error.txt
          -- $<TARGET_FILE:quant_backtest>
             ${CMAKE_CURRENT_LIST_DIR}/data/sample.csv -1.0 10.0)
  add_test(NAME ch17_cli_reports_the_multi_asset_valuation_boundary
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../tools/expect_process.py
          --expected-exit 2
          --expected-stderr
            ${CMAKE_CURRENT_LIST_DIR}/expected/mixed_symbols_error.txt
          -- $<TARGET_FILE:quant_backtest>
             ${CMAKE_CURRENT_LIST_DIR}/data/mixed_symbols.csv)
  add_test(NAME ch17_backtest_benchmark_preserves_the_hand_ledger
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../tools/validate_ch17_benchmark.py
          -- $<TARGET_FILE:ch17_backtest_benchmark>)
  add_test(NAME ch17_build_mixed_symbol_failure
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
          --target ch17_mixed_symbol_failure --config $<CONFIG>)
  set_tests_properties(ch17_build_mixed_symbol_failure PROPERTIES
  FIXTURES_SETUP ch17_mixed_symbol_failure_fixture)
  add_test(NAME ch17_mixed_symbols_report_the_valuation_boundary
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../tools/expect_process.py
          --expected-exit 2
          --expected-stderr
            ${CMAKE_CURRENT_LIST_DIR}/expected/mixed_symbols_error.txt
          -- $<TARGET_FILE:ch17_mixed_symbol_failure>)
  set_tests_properties(ch17_mixed_symbols_report_the_valuation_boundary
  PROPERTIES FIXTURES_REQUIRED ch17_mixed_symbol_failure_fixture)
endif()
