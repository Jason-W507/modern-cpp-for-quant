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
quant_target(ch03_basis_points_solution
  code/ch03/exercises/basis_points_solution.cpp)
quant_target(ch04_containers code/ch04/containers.cpp)
quant_target(ch04_algorithms code/ch04/algorithms.cpp)
quant_target(ch04_iterator_invalidation code/ch04/iterator_invalidation.cpp)
quant_target(ch04_csv_stats code/ch04/src/main.cpp)
target_sources(ch04_csv_stats PRIVATE code/ch04/src/csv_stats.cpp)
target_include_directories(ch04_csv_stats PRIVATE code/ch04/include)
quant_target(ch04_csv_stats_contracts code/ch04/csv_stats_contracts.cpp)
target_sources(ch04_csv_stats_contracts PRIVATE code/ch04/src/csv_stats.cpp)
target_include_directories(ch04_csv_stats_contracts PRIVATE code/ch04/include)
quant_target(ch04_symbol_summary_solution
  code/ch04/exercises/symbol_summary_solution.cpp)
target_sources(ch04_symbol_summary_solution PRIVATE code/ch04/src/csv_stats.cpp)
target_include_directories(ch04_symbol_summary_solution PRIVATE code/ch04/include)
quant_target(ch05_market_analyzer code/ch05/src/main.cpp)
target_sources(ch05_market_analyzer PRIVATE
  code/ch05/src/csv_reader.cpp
  code/ch05/src/market.cpp)
target_include_directories(ch05_market_analyzer PRIVATE code/ch05/include)
quant_target(ch05_records code/ch05/records.cpp)
quant_target(ch05_class_invariants code/ch05/class_invariants.cpp)
target_sources(ch05_class_invariants PRIVATE code/ch05/src/market.cpp)
target_include_directories(ch05_class_invariants PRIVATE code/ch05/include)
quant_target(ch05_price_range_solution
  code/ch05/exercises/price_range_solution.cpp)
target_sources(ch05_price_range_solution PRIVATE code/ch05/src/market.cpp)
target_include_directories(ch05_price_range_solution PRIVATE code/ch05/include)
quant_target(ch06_lifetime_trace code/ch06/lifetime_trace.cpp)
quant_target(ch06_borrowing_and_legacy code/ch06/borrowing_and_legacy.cpp)
quant_target(ch06_raii_file code/ch06/raii_file.cpp)
quant_target(ch06_ownership_choices code/ch06/ownership_choices.cpp)
quant_target(ch07_ranges_pipeline code/ch07/src/main.cpp)
target_sources(ch07_ranges_pipeline PRIVATE
  code/ch07/src/csv_reader.cpp
  code/ch07/src/pipeline.cpp)
target_include_directories(ch07_ranges_pipeline PRIVATE code/ch07/include)
quant_target(ch07_lazy_views code/ch07/lazy_views.cpp)
quant_target(ch07_reduction_order code/ch07/reduction_order.cpp)
quant_target(ch07_top_prices_solution
  code/ch07/exercises/top_prices_solution.cpp)
target_include_directories(ch07_top_prices_solution PRIVATE code/ch07/include)
quant_target(ch07_container_choices code/ch07/container_choices.cpp)
target_include_directories(ch07_container_choices PRIVATE code/ch07/include)

quant_target(ch08_test_strategy code/ch08/tests/test_strategy.cpp)
target_sources(ch08_test_strategy PRIVATE
  code/ch08/src/domain.cpp code/ch08/tests/test_support.cpp)
target_include_directories(ch08_test_strategy PRIVATE
  code/ch08/include code/ch08/tests)
add_test(NAME ch08_strategy_decision_seam COMMAND ch08_test_strategy)

quant_target(ch08_test_execution_portfolio
  code/ch08/tests/test_execution_portfolio.cpp)
target_sources(ch08_test_execution_portfolio PRIVATE
  code/ch08/src/domain.cpp code/ch08/tests/test_support.cpp)
target_include_directories(ch08_test_execution_portfolio PRIVATE
  code/ch08/include code/ch08/tests)
add_test(NAME ch08_execution_portfolio_seam
  COMMAND ch08_test_execution_portfolio)

quant_target(ch08_domain_pipeline code/ch08/src/main.cpp)
target_sources(ch08_domain_pipeline PRIVATE
  code/ch08/src/domain.cpp code/ch08/src/event_reader.cpp)
target_include_directories(ch08_domain_pipeline PRIVATE code/ch08/include)
quant_target(ch08_runtime_strategy code/ch08/runtime_strategy.cpp)
target_sources(ch08_runtime_strategy PRIVATE code/ch08/src/domain.cpp)
target_include_directories(ch08_runtime_strategy PRIVATE code/ch08/include)
quant_target(ch08_test_invalid_values code/ch08/tests/test_invalid_values.cpp)
target_sources(ch08_test_invalid_values PRIVATE
  code/ch08/src/domain.cpp code/ch08/tests/test_support.cpp)
target_include_directories(ch08_test_invalid_values PRIVATE
  code/ch08/include code/ch08/tests)
quant_target(ch08_sell_down_solution
  code/ch08/exercises/sell_down_solution.cpp)
target_sources(ch08_sell_down_solution PRIVATE code/ch08/src/domain.cpp)
target_include_directories(ch08_sell_down_solution PRIVATE code/ch08/include)

quant_target(ch09_test_static_strategy
  code/ch09/tests/test_static_strategy.cpp)
target_sources(ch09_test_static_strategy PRIVATE
  code/ch09/tests/test_support.cpp code/ch09/src/domain.cpp)
target_include_directories(ch09_test_static_strategy PRIVATE
  code/ch09/include code/ch09/tests)
add_test(NAME ch09_static_strategy_decision_seam
  COMMAND ch09_test_static_strategy)

quant_target(ch09_ordinary_strategy code/ch09/ordinary_strategy.cpp)
target_sources(ch09_ordinary_strategy PRIVATE code/ch09/src/domain.cpp)
target_include_directories(ch09_ordinary_strategy PRIVATE code/ch09/include)
quant_target(ch09_function_template code/ch09/function_template.cpp)
target_sources(ch09_function_template PRIVATE code/ch09/src/domain.cpp)
target_include_directories(ch09_function_template PRIVATE code/ch09/include)
quant_target(ch09_constrained_pipeline code/ch09/src/main.cpp)
target_sources(ch09_constrained_pipeline PRIVATE code/ch09/src/domain.cpp)
target_include_directories(ch09_constrained_pipeline PRIVATE code/ch09/include)
quant_target(ch09_polymorphism_compare code/ch09/polymorphism_compare.cpp)
target_sources(ch09_polymorphism_compare PRIVATE code/ch09/src/domain.cpp)
target_include_directories(ch09_polymorphism_compare PRIVATE code/ch09/include)
quant_target(ch09_maximum_position_solution
  code/ch09/exercises/maximum_position_solution.cpp)
target_sources(ch09_maximum_position_solution PRIVATE code/ch09/src/domain.cpp)
target_include_directories(ch09_maximum_position_solution PRIVATE
  code/ch09/include)

add_test(NAME ch09_ordinary_strategy_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch09/expected/ordinary_strategy.txt
          -- $<TARGET_FILE:ch09_ordinary_strategy>)
add_test(NAME ch09_function_template_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch09/expected/function_template.txt
          -- $<TARGET_FILE:ch09_function_template>)
add_test(NAME ch09_constrained_pipeline_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch09/expected/constrained_pipeline.txt
          -- $<TARGET_FILE:ch09_constrained_pipeline>)
add_test(NAME ch09_polymorphism_compare_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch09/expected/polymorphism_compare.txt
          -- $<TARGET_FILE:ch09_polymorphism_compare>)
add_test(NAME ch09_maximum_position_solution_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch09/expected/maximum_position_solution.txt
          -- $<TARGET_FILE:ch09_maximum_position_solution>)
add_test(NAME ch09_unconstrained_error_exposes_missing_member
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_compile_failure.py
          --compiler ${CMAKE_CXX_COMPILER}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch09/failures/unconstrained_strategy.cpp
          --output ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch09_unconstrained.obj
          --diagnostic "request for member.*on_event|has no member.*on_event")
add_test(NAME ch09_concept_error_reports_unsatisfied_constraint
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_compile_failure.py
          --compiler ${CMAKE_CXX_COMPILER}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch09/failures/constrained_strategy.cpp
          --output ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch09_constrained.obj
          --diagnostic "constraints not satisfied|constraint.*not satisfied")
add_test(NAME ch09_clean_constrained_pipeline_configures_builds_and_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/test_clean_cmake_project.py
          --cmake ${CMAKE_COMMAND}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch09
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch09/expected/constrained_pipeline.txt
          --target ch09_constrained_pipeline
          --generator "${CMAKE_GENERATOR}"
          --compiler ${CMAKE_CXX_COMPILER}
          --work-root ${CMAKE_CURRENT_BINARY_DIR}/clean-build-tests
          --config $<CONFIG>)

quant_target(ch10_test_atomic_portfolio
  code/ch10/tests/test_atomic_portfolio.cpp)
target_sources(ch10_test_atomic_portfolio PRIVATE
  code/ch10/src/domain.cpp code/ch10/src/portfolio.cpp
  code/ch10/tests/test_support.cpp)
target_include_directories(ch10_test_atomic_portfolio PRIVATE
  code/ch10/include code/ch10/tests)
add_test(NAME ch10_execution_portfolio_strong_guarantee_seam
  COMMAND ch10_test_atomic_portfolio)

quant_target(ch10_test_csv_errors code/ch10/tests/test_csv_errors.cpp)
target_sources(ch10_test_csv_errors PRIVATE
  code/ch10/src/csv_reader.cpp code/ch10/src/domain.cpp
  code/ch10/tests/test_support.cpp)
target_include_directories(ch10_test_csv_errors PRIVATE
  code/ch10/include code/ch10/tests)
add_test(NAME ch10_market_input_preserves_error_context_seam
  COMMAND ch10_test_csv_errors)

quant_target(ch10_test_observability code/ch10/tests/test_observability.cpp)
target_sources(ch10_test_observability PRIVATE
  code/ch10/src/csv_reader.cpp code/ch10/src/domain.cpp
  code/ch10/src/pipeline.cpp code/ch10/src/portfolio.cpp
  code/ch10/tests/test_support.cpp)
target_include_directories(ch10_test_observability PRIVATE
  code/ch10/include code/ch10/tests)
add_test(NAME ch10_boundary_logs_error_once_seam
  COMMAND ch10_test_observability)

quant_target(ch10_recovery_pipeline code/ch10/src/main.cpp)
target_sources(ch10_recovery_pipeline PRIVATE
  code/ch10/src/csv_reader.cpp code/ch10/src/domain.cpp
  code/ch10/src/pipeline.cpp code/ch10/src/portfolio.cpp)
target_include_directories(ch10_recovery_pipeline PRIVATE code/ch10/include)

quant_target(ch10_error_choices code/ch10/error_choices.cpp)
target_sources(ch10_error_choices PRIVATE
  code/ch10/src/csv_reader.cpp code/ch10/src/domain.cpp)
target_include_directories(ch10_error_choices PRIVATE code/ch10/include)

quant_target(ch10_error_injection_solution
  code/ch10/exercises/error_injection_solution.cpp)
target_sources(ch10_error_injection_solution PRIVATE
  code/ch10/src/domain.cpp code/ch10/src/portfolio.cpp)
target_include_directories(ch10_error_injection_solution PRIVATE
  code/ch10/include)

add_test(NAME ch10_recovery_pipeline_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_process.py
          --expected-exit 0
          --expected-stdout ${CMAKE_CURRENT_SOURCE_DIR}/code/ch10/expected/recovery_pipeline.txt
          --expected-stderr ${CMAKE_CURRENT_SOURCE_DIR}/code/ch10/expected/recovery_pipeline_errors.txt
          -- $<TARGET_FILE:ch10_recovery_pipeline>
             ${CMAKE_CURRENT_SOURCE_DIR}/code/ch10/data/fills.csv)
add_test(NAME ch10_external_failure_is_logged_once
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_process.py
          --expected-exit 2
          --expected-stderr ${CMAKE_CURRENT_SOURCE_DIR}/code/ch10/expected/missing_file_error.txt
          -- $<TARGET_FILE:ch10_recovery_pipeline>
             missing.csv)
add_test(NAME ch10_error_choices_run
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch10/expected/error_choices.txt
          -- $<TARGET_FILE:ch10_error_choices>)
add_test(NAME ch10_error_injection_solution_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch10/expected/error_injection_solution.txt
          -- $<TARGET_FILE:ch10_error_injection_solution>)
add_test(NAME ch10_clean_recovery_pipeline_configures_builds_and_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/test_clean_cmake_project.py
          --cmake ${CMAKE_COMMAND}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch10
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch10/expected/recovery_pipeline.txt
          --expected-stderr ${CMAKE_CURRENT_SOURCE_DIR}/code/ch10/expected/recovery_pipeline_errors.txt
          --target ch10_recovery_pipeline
          --argument ${CMAKE_CURRENT_SOURCE_DIR}/code/ch10/data/fills.csv
          --generator "${CMAKE_GENERATOR}"
          --compiler ${CMAKE_CXX_COMPILER}
          --work-root ${CMAKE_CURRENT_BINARY_DIR}/clean-build-tests
          --config $<CONFIG>)

add_test(NAME ch11_clean_target_graph_configures_builds_and_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/test_clean_cmake_project.py
          --cmake ${CMAKE_COMMAND}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch11
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch11/expected/market_report.txt
          --target ch11_market_report
          --generator "${CMAKE_GENERATOR}"
          --compiler ${CMAKE_CXX_COMPILER}
          --work-root ${CMAKE_CURRENT_BINARY_DIR}/clean-build-tests/ch11-debug
          --config $<CONFIG>
          --build-all
          --ctest ${CMAKE_CTEST_COMMAND}
          --run-ctest)
add_test(NAME ch11_missing_target_dependency_fails_during_linking
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_link_failure.py
          --compiler ${CMAKE_CXX_COMPILER}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch11/failures/missing_target_link.cpp
          --work-dir ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch11-link
          --diagnostic "undefined reference|unresolved external symbol")
add_test(NAME ch11_sanitized_configuration_builds_and_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/test_clean_cmake_project.py
          --cmake ${CMAKE_COMMAND}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch11
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch11/expected/market_report.txt
          --target ch11_market_report
          --generator "${CMAKE_GENERATOR}"
          --compiler ${CMAKE_CXX_COMPILER}
          --work-root ${CMAKE_CURRENT_BINARY_DIR}/clean-build-tests/ch11-sanitized
          --config $<CONFIG>
          --cmake-argument=-DCMAKE_BUILD_TYPE=Debug
          --cmake-argument=-DCH11_ENABLE_SANITIZERS=ON
          --build-all
          --ctest ${CMAKE_CTEST_COMMAND}
          --run-ctest)

add_test(NAME ch12_clean_quality_lab_configures_builds_and_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/test_clean_cmake_project.py
          --cmake ${CMAKE_COMMAND}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch12
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch12/expected/portfolio_test.txt
          --target ch12_portfolio_tests
          --generator "${CMAKE_GENERATOR}"
          --compiler ${CMAKE_CXX_COMPILER}
          --work-root ${CMAKE_CURRENT_BINARY_DIR}/clean-build-tests/ch12-debug
          --config $<CONFIG>
          --build-all
          --ctest ${CMAKE_CTEST_COMMAND}
          --run-ctest)
add_test(NAME ch12_release_assertion_trap_is_observable
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/test_clean_cmake_project.py
          --cmake ${CMAKE_COMMAND}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch12
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch12/expected/assert_only_release.txt
          --target ch12_assert_only_release
          --generator "${CMAKE_GENERATOR}"
          --compiler ${CMAKE_CXX_COMPILER}
          --work-root ${CMAKE_CURRENT_BINARY_DIR}/clean-build-tests/ch12-assert-release
          --config Release
          --cmake-argument=-DCMAKE_BUILD_TYPE=Release)
add_test(NAME ch12_release_real_checks_stay_active
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/test_clean_cmake_project.py
          --cmake ${CMAKE_COMMAND}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch12
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch12/expected/portfolio_test.txt
          --target ch12_portfolio_tests
          --generator "${CMAKE_GENERATOR}"
          --compiler ${CMAKE_CXX_COMPILER}
          --work-root ${CMAKE_CURRENT_BINARY_DIR}/clean-build-tests/ch12-release
          --config Release
          --cmake-argument=-DCMAKE_BUILD_TYPE=Release
          --build-all
          --ctest ${CMAKE_CTEST_COMMAND}
          --run-ctest)
add_test(NAME ch12_out_of_bounds_is_reported_by_address_sanitizer
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_sanitizer_failure.py
          --compiler ${CMAKE_CXX_COMPILER}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch12/failures/out_of_bounds.cpp
          --work-dir ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch12-bounds
          --sanitizer address
          --diagnostic "heap-buffer-overflow")
set_tests_properties(ch12_out_of_bounds_is_reported_by_address_sanitizer
  PROPERTIES SKIP_RETURN_CODE 77)
add_test(NAME ch12_dangling_access_is_reported_by_address_sanitizer
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_sanitizer_failure.py
          --compiler ${CMAKE_CXX_COMPILER}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch12/failures/dangling_pointer.cpp
          --work-dir ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch12-dangling
          --sanitizer address
          --diagnostic "heap-use-after-free")
set_tests_properties(ch12_dangling_access_is_reported_by_address_sanitizer
  PROPERTIES SKIP_RETURN_CODE 77)
add_test(NAME ch12_signed_overflow_is_reported_by_undefined_sanitizer
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_sanitizer_failure.py
          --compiler ${CMAKE_CXX_COMPILER}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch12/failures/signed_overflow.cpp
          --work-dir ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch12-overflow
          --sanitizer undefined
          --diagnostic "signed integer overflow")
set_tests_properties(ch12_signed_overflow_is_reported_by_undefined_sanitizer
  PROPERTIES SKIP_RETURN_CODE 77)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_test(NAME ch12_double_delete_is_reported_by_static_analyzer
    COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_compile_diagnostic.py
            --compiler ${CMAKE_CXX_COMPILER}
            --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch12/failures/double_delete.cpp
            --output ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch12_double_delete.obj
            --compiler-argument=-fanalyzer
            --diagnostic "double.*free|use after.*delete")
endif()
add_test(NAME ch12_logic_bug_is_caught_by_behavior_oracle
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_runtime_failure.py
          --compiler ${CMAKE_CXX_COMPILER}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch12/failures/logic_bug.cpp
          --work-dir ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch12-logic
          --expected-exit 2
          --diagnostic "observed-cash=801 expected-cash=799")

add_test(NAME ch13_clean_layout_snapshot_configures_builds_and_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/test_clean_cmake_project.py
          --cmake ${CMAKE_COMMAND}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch13
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch13/expected/quote_scan.txt
          --target ch13_quote_scan
          --generator "${CMAKE_GENERATOR}"
          --compiler ${CMAKE_CXX_COMPILER}
          --work-root ${CMAKE_CURRENT_BINARY_DIR}/clean-build-tests/ch13
          --config $<CONFIG>
          --build-all
          --ctest ${CMAKE_CTEST_COMMAND}
          --run-ctest)
add_test(NAME ch13_arena_dangling_is_reported_by_address_sanitizer
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_sanitizer_failure.py
          --compiler ${CMAKE_CXX_COMPILER}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch13/failures/arena_dangling.cpp
          --work-dir ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch13-arena
          --sanitizer address
          --diagnostic "heap-use-after-free")
set_tests_properties(ch13_arena_dangling_is_reported_by_address_sanitizer
  PROPERTIES SKIP_RETURN_CODE 77)

add_test(NAME ch08_domain_pipeline_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch08/expected/domain_pipeline.txt
          -- $<TARGET_FILE:ch08_domain_pipeline>
             ${CMAKE_CURRENT_SOURCE_DIR}/code/ch08/data/events.csv)
add_test(NAME ch08_runtime_strategy_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch08/expected/runtime_strategy.txt
          -- $<TARGET_FILE:ch08_runtime_strategy>)
add_test(NAME ch08_invalid_values_are_rejected
  COMMAND ch08_test_invalid_values)
set_tests_properties(ch08_invalid_values_are_rejected PROPERTIES
  PASS_REGULAR_EXPRESSION "domain invariants ok")
add_test(NAME ch08_sell_down_solution_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch08/expected/sell_down_solution.txt
          -- $<TARGET_FILE:ch08_sell_down_solution>)
add_test(NAME ch08_wrong_override_fails_during_compilation
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_compile_failure.py
          --compiler ${CMAKE_CXX_COMPILER}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch08/failures/wrong_override.cpp
          --output ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch08_wrong_override.obj
          --diagnostic "does not override|marked.*override|override.*but does not")
add_test(NAME ch08_clean_domain_pipeline_configures_builds_and_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/test_clean_cmake_project.py
          --cmake ${CMAKE_COMMAND}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch08
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch08/expected/domain_pipeline.txt
          --target ch08_domain_pipeline
          --argument ${CMAKE_CURRENT_SOURCE_DIR}/code/ch08/data/events.csv
          --generator "${CMAKE_GENERATOR}"
          --compiler ${CMAKE_CXX_COMPILER}
          --work-root ${CMAKE_CURRENT_BINARY_DIR}/clean-build-tests
          --config $<CONFIG>)

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
add_subdirectory(code/ch11 ch11-snapshot)
add_subdirectory(code/ch12 ch12-snapshot)
add_subdirectory(code/ch13 ch13-snapshot)
add_test(NAME layout_benchmark_smoke COMMAND layout_benchmark)
set_tests_properties(layout_benchmark_smoke PROPERTIES
  PASS_REGULAR_EXPRESSION "checksum-match=true")
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
add_test(NAME ch03_basis_points_solution_converts_rate_without_mutation
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch03/expected/basis_points_solution.txt
          -- $<TARGET_FILE:ch03_basis_points_solution>)
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
          --diagnostic "reference to local variable|reference to local temporary|reference to stack memory|C4172")
add_test(NAME ch07_ranges_pipeline_summarizes_one_symbol
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_process.py
          --expected-exit 0
          --expected-stdout ${CMAKE_CURRENT_SOURCE_DIR}/code/ch07/expected/ranges_pipeline.txt
          -- $<TARGET_FILE:ch07_ranges_pipeline>
             ${CMAKE_CURRENT_SOURCE_DIR}/code/ch07/data/trades.csv
             AAPL)
add_test(NAME ch07_views_observe_late_changes_to_the_borrowed_range
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch07/expected/lazy_views.txt
          -- $<TARGET_FILE:ch07_lazy_views>)
add_test(NAME ch07_floating_reduction_exposes_order_dependence
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch07/expected/reduction_order.txt
          -- $<TARGET_FILE:ch07_reduction_order>)
add_test(NAME ch07_dangling_view_is_reported_by_address_sanitizer
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_asan_failure.py
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch07/failures/dangling_view.cpp
          --diagnostic "stack-use-after-return|stack-use-after-scope|stack-buffer-underflow")
set_tests_properties(ch07_dangling_view_is_reported_by_address_sanitizer
  PROPERTIES SKIP_RETURN_CODE 77)
add_test(NAME ch07_top_prices_solution_filters_sorts_and_limits
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch07/expected/top_prices_solution.txt
          -- $<TARGET_FILE:ch07_top_prices_solution>)
add_test(NAME ch07_ranges_pipeline_rejects_a_missing_symbol
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_process.py
          --expected-exit 2
          --expected-stderr ${CMAKE_CURRENT_SOURCE_DIR}/code/ch07/expected/missing_symbol_error.txt
          -- $<TARGET_FILE:ch07_ranges_pipeline>
             ${CMAKE_CURRENT_SOURCE_DIR}/code/ch07/data/trades.csv
             TSLA)
add_test(NAME ch07_ranges_pipeline_rejects_a_symbol_without_sells
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_process.py
          --expected-exit 2
          --expected-stderr ${CMAKE_CURRENT_SOURCE_DIR}/code/ch07/expected/no_sell_error.txt
          -- $<TARGET_FILE:ch07_ranges_pipeline>
             ${CMAKE_CURRENT_SOURCE_DIR}/code/ch07/data/trades.csv
             MSFT)
add_test(NAME ch07_ranges_pipeline_rejects_an_invalid_side
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_process.py
          --expected-exit 2
          --expected-stderr ${CMAKE_CURRENT_SOURCE_DIR}/code/ch07/expected/invalid_side_error.txt
          -- $<TARGET_FILE:ch07_ranges_pipeline>
             ${CMAKE_CURRENT_SOURCE_DIR}/code/ch07/data/invalid_side.csv
             AAPL)
add_test(NAME ch07_clean_ranges_pipeline_configures_builds_and_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/test_clean_cmake_project.py
          --cmake ${CMAKE_COMMAND}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch07
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch07/expected/ranges_pipeline.txt
          --target ch07_ranges_pipeline
          --argument ${CMAKE_CURRENT_SOURCE_DIR}/code/ch07/data/trades.csv
          --argument AAPL
          --generator "${CMAKE_GENERATOR}"
          --compiler ${CMAKE_CXX_COMPILER}
          --work-root ${CMAKE_CURRENT_BINARY_DIR}/clean-build-tests
          --config $<CONFIG>)
add_test(NAME ch07_container_choices_match_access_patterns
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch07/expected/container_choices.txt
          -- $<TARGET_FILE:ch07_container_choices>)
add_test(NAME ch04_containers_expose_sizes_and_values
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch04/expected/containers.txt
          -- $<TARGET_FILE:ch04_containers>)
add_test(NAME ch04_algorithms_match_the_manual_loop
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch04/expected/algorithms.txt
          -- $<TARGET_FILE:ch04_algorithms>)
add_test(NAME ch04_iterators_are_reacquired_after_reallocation
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch04/expected/iterator_invalidation.txt
          -- $<TARGET_FILE:ch04_iterator_invalidation>)
add_test(NAME ch04_stale_iterator_is_reported_by_address_sanitizer
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_asan_failure.py
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch04/failures/stale_iterator.cpp
          --diagnostic "heap-use-after-free")
set_tests_properties(ch04_stale_iterator_is_reported_by_address_sanitizer
  PROPERTIES SKIP_RETURN_CODE 77)
add_test(NAME ch04_csv_stats_summarizes_valid_market_rows
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_process.py
          --expected-exit 0
          --expected-stdout ${CMAKE_CURRENT_SOURCE_DIR}/code/ch04/expected/csv_stats.txt
          -- $<TARGET_FILE:ch04_csv_stats>
             ${CMAKE_CURRENT_SOURCE_DIR}/code/ch04/data/market_rows.csv)
add_test(NAME ch04_csv_stats_handles_stream_and_shape_boundaries
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_process.py
          --expected-exit 0
          --expected-stdout ${CMAKE_CURRENT_SOURCE_DIR}/code/ch04/expected/csv_stats_contracts.txt
          -- $<TARGET_FILE:ch04_csv_stats_contracts>)
add_test(NAME ch04_clean_csv_project_configures_builds_and_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/test_clean_cmake_project.py
          --cmake ${CMAKE_COMMAND}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch04
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch04/expected/csv_stats.txt
          --target ch04_csv_stats
          --argument ${CMAKE_CURRENT_SOURCE_DIR}/code/ch04/data/market_rows.csv
          --generator "${CMAKE_GENERATOR}"
          --compiler ${CMAKE_CXX_COMPILER}
          --work-root ${CMAKE_CURRENT_BINARY_DIR}/clean-build-tests
          --config $<CONFIG>)
add_test(NAME ch04_csv_stats_rejects_an_empty_symbol
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_process.py
          --expected-exit 2
          --expected-stderr ${CMAKE_CURRENT_SOURCE_DIR}/code/ch04/expected/empty_symbol_error.txt
          -- $<TARGET_FILE:ch04_csv_stats>
             ${CMAKE_CURRENT_SOURCE_DIR}/code/ch04/data/empty_symbol.csv)
add_test(NAME ch04_csv_stats_rejects_an_invalid_number
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_process.py
          --expected-exit 2
          --expected-stderr ${CMAKE_CURRENT_SOURCE_DIR}/code/ch04/expected/invalid_price_error.txt
          -- $<TARGET_FILE:ch04_csv_stats>
             ${CMAKE_CURRENT_SOURCE_DIR}/code/ch04/data/invalid_price.csv)
add_test(NAME ch04_csv_stats_rejects_the_wrong_column_count
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_process.py
          --expected-exit 2
          --expected-stderr ${CMAKE_CURRENT_SOURCE_DIR}/code/ch04/expected/wrong_columns_error.txt
          -- $<TARGET_FILE:ch04_csv_stats>
             ${CMAKE_CURRENT_SOURCE_DIR}/code/ch04/data/wrong_columns.csv)
add_test(NAME ch04_symbol_summary_solution_filters_one_symbol
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch04/expected/symbol_summary_solution.txt
          -- $<TARGET_FILE:ch04_symbol_summary_solution>
             ${CMAKE_CURRENT_SOURCE_DIR}/code/ch04/data/market_rows.csv
             AAPL)
add_test(NAME ch05_market_analyzer_summarizes_one_symbol
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_process.py
          --expected-exit 0
          --expected-stdout ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/expected/market_analyzer.txt
          -- $<TARGET_FILE:ch05_market_analyzer>
             ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/data/market_rows.csv
             AAPL)
add_test(NAME ch05_market_analyzer_rejects_an_invalid_quote
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_process.py
          --expected-exit 2
          --expected-stderr ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/expected/invalid_price_error.txt
          -- $<TARGET_FILE:ch05_market_analyzer>
             ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/data/invalid_price.csv
             AAPL)
add_test(NAME ch05_market_analyzer_rejects_an_empty_symbol
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_process.py
          --expected-exit 2
          --expected-stderr ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/expected/empty_symbol_error.txt
          -- $<TARGET_FILE:ch05_market_analyzer>
             ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/data/empty_symbol.csv
             AAPL)
add_test(NAME ch05_market_analyzer_rejects_a_nonpositive_quantity
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_process.py
          --expected-exit 2
          --expected-stderr ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/expected/invalid_quantity_error.txt
          -- $<TARGET_FILE:ch05_market_analyzer>
             ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/data/invalid_quantity.csv
             AAPL)
add_test(NAME ch05_market_analyzer_rejects_a_symbol_without_rows
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_process.py
          --expected-exit 2
          --expected-stderr ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/expected/missing_symbol_error.txt
          -- $<TARGET_FILE:ch05_market_analyzer>
             ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/data/market_rows.csv
             TSLA)
add_test(NAME ch05_market_analyzer_preserves_numeric_error_context
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_process.py
          --expected-exit 2
          --expected-stderr ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/expected/invalid_number_error.txt
          -- $<TARGET_FILE:ch05_market_analyzer>
             ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/data/invalid_number.csv
             AAPL)
add_test(NAME ch05_market_analyzer_rejects_a_nonnumeric_quantity
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_process.py
          --expected-exit 2
          --expected-stderr ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/expected/invalid_quantity_text_error.txt
          -- $<TARGET_FILE:ch05_market_analyzer>
             ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/data/invalid_quantity_text.csv
             AAPL)
add_test(NAME ch05_market_analyzer_rejects_the_wrong_header
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_process.py
          --expected-exit 2
          --expected-stderr ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/expected/wrong_header_error.txt
          -- $<TARGET_FILE:ch05_market_analyzer>
             ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/data/wrong_header.csv
             AAPL)
add_test(NAME ch05_market_analyzer_rejects_the_wrong_column_count
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_process.py
          --expected-exit 2
          --expected-stderr ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/expected/wrong_columns_error.txt
          -- $<TARGET_FILE:ch05_market_analyzer>
             ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/data/wrong_columns.csv
             AAPL)
add_test(NAME ch05_market_analyzer_reports_usage_errors
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_process.py
          --expected-exit 2
          --expected-stderr ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/expected/usage_error.txt
          -- $<TARGET_FILE:ch05_market_analyzer>)
add_test(NAME ch05_market_analyzer_reports_file_open_errors
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_process.py
          --expected-exit 2
          --expected-stderr ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/expected/open_error.txt
          -- $<TARGET_FILE:ch05_market_analyzer>
             ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/data/does-not-exist.csv
             AAPL)
add_test(NAME ch05_public_record_exposes_aggregate_state
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/expected/records.txt
          -- $<TARGET_FILE:ch05_records>)
add_test(NAME ch05_class_constructor_preserves_quote_invariants
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/expected/class_invariants.txt
          -- $<TARGET_FILE:ch05_class_invariants>)
add_test(NAME ch05_private_state_is_rejected_at_compile_time
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_compile_failure.py
          --compiler ${CMAKE_CXX_COMPILER}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/failures/private_access.cpp
          --output ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch05_private_access.obj
          --diagnostic "private|inaccessible")
add_test(NAME ch05_price_range_solution_reuses_the_analyzer
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output.py
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/expected/price_range_solution.txt
          -- $<TARGET_FILE:ch05_price_range_solution>)
add_test(NAME ch05_clean_market_analyzer_configures_builds_and_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/test_clean_cmake_project.py
          --cmake ${CMAKE_COMMAND}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/expected/market_analyzer.txt
          --target ch05_market_analyzer
          --argument ${CMAKE_CURRENT_SOURCE_DIR}/code/ch05/data/market_rows.csv
          --argument AAPL
          --generator "${CMAKE_GENERATOR}"
          --compiler ${CMAKE_CXX_COMPILER}
          --work-root ${CMAKE_CURRENT_BINARY_DIR}/clean-build-tests
          --config $<CONFIG>)
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
