cmake_minimum_required(VERSION 3.20)
project(modern_cpp_for_quant VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

function(quant_target target source)
  add_executable(${target} ${source})
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4 /permissive-)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endfunction()

function(quant_project_target target source)
  quant_target(${target} ${source})
  target_include_directories(${target} PRIVATE project/include project/tests)
endfunction()

quant_target(ch01_first_program code/ch01/first_program.cpp)
quant_target(ch02_objects_and_literals code/ch02/objects_and_literals.cpp)
quant_target(ch02_expressions code/ch02/expressions.cpp)
quant_target(ch02_branches code/ch02/branches.cpp)
quant_target(ch02_for_scope code/ch02/for_scope.cpp)
quant_target(ch02_market_filter code/ch02/market_filter.cpp)
quant_target(ch02_risk_budget_solution code/ch02/exercises/risk_budget_solution.cpp)
quant_target(ch03_market_tool code/ch03/src/main.cpp)
target_sources(ch03_market_tool PRIVATE code/ch03/src/market_math.cpp)
target_include_directories(ch03_market_tool PRIVATE code/ch03/include)
quant_target(ch03_parameter_modes code/ch03/parameter_modes.cpp)
quant_project_target(ch04_ranges_pipeline code/ch04/ranges_pipeline.cpp)
quant_target(ch06_lifetime_trace code/ch06/lifetime_trace.cpp)
quant_target(ch06_borrowing_and_legacy code/ch06/borrowing_and_legacy.cpp)
quant_target(ch06_raii_file code/ch06/raii_file.cpp)
quant_target(ch06_ownership_choices code/ch06/ownership_choices.cpp)

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

quant_target(layout_benchmark project/benchmarks/layout_benchmark.cpp)
quant_target(ch11_parallel_sum code/ch11/parallel_sum.cpp)
quant_target(ch12_numerical_stability code/ch12/numerical_stability.cpp)
quant_target(ch12_pybind_boundary code/ch12/pybind11_boundary.cpp)
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

add_test(NAME ch01_first_program_prints_exact_readiness_message
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch01/expected/first_program.txt
          -- $<TARGET_FILE:ch01_first_program>)
add_test(NAME ch01_clean_cmake_project_configures_builds_and_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/test_clean_cmake_project.py
          --cmake ${CMAKE_COMMAND}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch01
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch01/expected/first_program.txt
          --generator "${CMAKE_GENERATOR}"
          --compiler ${CMAKE_CXX_COMPILER}
          --work-root ${CMAKE_CURRENT_BINARY_DIR}/clean-build-tests
          --config $<CONFIG>)
add_test(NAME ch01_missing_semicolon_fails_during_compilation
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_compile_failure.py
          --compiler ${CMAKE_CXX_COMPILER}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch01/failures/missing_semicolon.cpp
          --output ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch01_missing_semicolon.obj
          --diagnostic "expected.*;|missing.*;")
add_test(NAME ch01_missing_definition_fails_during_linking
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_link_failure.py
          --compiler ${CMAKE_CXX_COMPILER}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch01/failures/missing_definition.cpp
          --work-dir ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch01-link
          --diagnostic "undefined reference|unresolved external symbol")
add_test(NAME ch01_startup_failure_is_observed_at_runtime
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_runtime_failure.py
          --compiler ${CMAKE_CXX_COMPILER}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch01/failures/startup_failure.cpp
          --work-dir ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch01-runtime
          --expected-exit 2
          --diagnostic "startup-error: market-data path is required")
add_test(NAME ch02_objects_and_literals_show_independent_values
  COMMAND ch02_objects_and_literals)
set_tests_properties(ch02_objects_and_literals_show_independent_values PROPERTIES
  PASS_REGULAR_EXPRESSION
    "side=1 price=100.5 quantity=10 buy=1 venue=X changed=11 copied=10")
add_test(NAME ch02_expressions_preserve_fractional_results COMMAND ch02_expressions)
set_tests_properties(ch02_expressions_preserve_fractional_results PROPERTIES
  PASS_REGULAR_EXPRESSION "notional=1005 average=502.5 integer_division=2")
add_test(NAME ch02_branches_select_valid_buy COMMAND ch02_branches)
set_tests_properties(ch02_branches_select_valid_buy PROPERTIES
  PASS_REGULAR_EXPRESSION "selected")
add_test(NAME ch02_for_scope_counts_even_rows COMMAND ch02_for_scope)
set_tests_properties(ch02_for_scope_counts_even_rows PROPERTIES
  PASS_REGULAR_EXPRESSION "selected=3")
add_test(NAME ch02_market_filter_summarizes_valid_buys
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/run_with_input.py
          --input ${CMAKE_CURRENT_SOURCE_DIR}/code/ch02/data/market_rows.txt
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch02/expected/market_filter.txt
          -- $<TARGET_FILE:ch02_market_filter>)
add_test(NAME ch02_risk_budget_solution_counts_affordable_trades
  COMMAND ch02_risk_budget_solution)
set_tests_properties(ch02_risk_budget_solution_counts_affordable_trades PROPERTIES
  PASS_REGULAR_EXPRESSION "accepted_trades=4 remaining_budget=200")
add_test(NAME ch02_narrowing_is_rejected_at_compile_time
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_compile_failure.py
          --compiler ${CMAKE_CXX_COMPILER}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch02/failures/narrowing.cpp
          --output ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch02_narrowing.obj
          --diagnostic "narrowing|narrowed")
add_test(NAME ch03_market_tool_summarizes_fixed_rows
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/run_with_input.py
          --input ${CMAKE_CURRENT_SOURCE_DIR}/code/ch03/data/market_rows.txt
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch03/expected/market_tool.txt
          -- $<TARGET_FILE:ch03_market_tool>)
add_test(NAME ch03_clean_multifile_project_configures_builds_and_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/test_clean_cmake_project.py
          --cmake ${CMAKE_COMMAND}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch03
          --input ${CMAKE_CURRENT_SOURCE_DIR}/code/ch03/data/market_rows.txt
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch03/expected/market_tool.txt
          --target ch03_market_tool
          --generator "${CMAKE_GENERATOR}"
          --compiler ${CMAKE_CXX_COMPILER}
          --work-root ${CMAKE_CURRENT_BINARY_DIR}/clean-build-tests
          --config $<CONFIG>)
add_test(NAME ch03_parameter_modes_preserve_or_modify_callers_as_declared
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch03/expected/parameter_modes.txt
          -- $<TARGET_FILE:ch03_parameter_modes>)
add_test(NAME ch03_missing_definition_fails_during_linking
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_link_failure.py
          --compiler ${CMAKE_CXX_COMPILER}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch03/failures/missing_definition.cpp
          --work-dir ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch03-missing
          --diagnostic "undefined reference|unresolved external symbol")
add_test(NAME ch03_duplicate_definition_fails_during_linking
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_multi_source_link_failure.py
          --compiler ${CMAKE_CXX_COMPILER}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch03/failures/duplicate_definition_a.cpp
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch03/failures/duplicate_definition_b.cpp
          --work-dir ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch03-duplicate
          --diagnostic "multiple definition|already defined")
add_test(NAME ch03_dangling_reference_emits_compile_diagnostic
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_compile_diagnostic.py
          --compiler ${CMAKE_CXX_COMPILER}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch03/failures/dangling_reference.cpp
          --output ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch03_dangling.obj
          --diagnostic "reference to local variable|reference to local temporary|C4172")
add_test(NAME ch04_ranges_pipeline_runs COMMAND ch04_ranges_pipeline)
set_tests_properties(ch04_ranges_pipeline_runs PROPERTIES
  PASS_REGULAR_EXPRESSION "positive-notional=3975")
add_test(NAME ch06_lifetime_trace_shows_copy_move_and_reverse_destruction
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch06/expected/lifetime_trace.txt
          -- $<TARGET_FILE:ch06_lifetime_trace>)
add_test(NAME ch06_borrowing_and_legacy_distinguishes_observation_from_ownership
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch06/expected/borrowing_and_legacy.txt
          -- $<TARGET_FILE:ch06_borrowing_and_legacy>)
add_test(NAME ch06_raii_file_closes_at_scope_exit
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch06/expected/raii_file.txt
          -- $<TARGET_FILE:ch06_raii_file>)
add_test(NAME ch06_ownership_choices_show_value_unique_shared_and_weak_states
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch06/expected/ownership_choices.txt
          -- $<TARGET_FILE:ch06_ownership_choices>)
add_test(NAME ch06_use_after_free_is_reported_by_address_sanitizer
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_asan_failure.py
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch06/failures/use_after_free.cpp
          --diagnostic "heap-use-after-free")
set_tests_properties(ch06_use_after_free_is_reported_by_address_sanitizer PROPERTIES
  SKIP_RETURN_CODE 77)

add_test(NAME book_contract
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/check_book_contract.py
          --root ${CMAKE_CURRENT_SOURCE_DIR})
set_tests_properties(book_contract PROPERTIES
  PASS_REGULAR_EXPRESSION "18 chapters.*150 planned pages")

add_test(NAME book_contract_rejects_invalid_authoring_evidence
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/tests/test_book_contract.py)

add_test(NAME output_harness_rejects_whitespace_drift
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/tests/test_output_contract.py)
