include_guard(GLOBAL)

if(NOT BUILD_TESTING)
  return()
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_test(NAME ch17_clean_final_backtest_configures_builds_tests_and_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/test_clean_cmake_project.py
          --cmake ${CMAKE_COMMAND}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/project
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/project/expected/sample_backtest.txt
          --target quant_backtest
          --argument ${CMAKE_CURRENT_SOURCE_DIR}/project/data/sample.csv
          --generator "${CMAKE_GENERATOR}"
          --compiler ${CMAKE_CXX_COMPILER}
          --work-root ${CMAKE_CURRENT_BINARY_DIR}/clean-build-tests
          --config $<CONFIG>
          --build-all
          --ctest ${CMAKE_CTEST_COMMAND}
          --run-ctest)
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
  PASS_REGULAR_EXPRESSION "18 chapters.*235 planned pages")

add_test(NAME book_contract_rejects_invalid_authoring_evidence
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/tests/test_book_contract.py)

add_test(NAME output_harness_rejects_whitespace_drift
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/tests/test_output_contract.py)

add_test(NAME version_contract_has_one_source
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/tests/test_version_contract.py)

add_test(NAME public_repository_contract
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/tests/test_public_repository_contract.py)

add_test(NAME cmake_architecture_contract
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/tests/test_cmake_architecture.py)

add_test(NAME release_package_contract
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/tests/test_release_package.py)

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
if(NOT WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
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
endif()

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

add_test(NAME ch14_clean_measurement_lab_configures_builds_and_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/test_clean_cmake_project.py
          --cmake ${CMAKE_COMMAND}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch14
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch14/expected/statistics_report.txt
          --target ch14_statistics_report
          --generator "${CMAKE_GENERATOR}"
          --compiler ${CMAKE_CXX_COMPILER}
          --work-root ${CMAKE_CURRENT_BINARY_DIR}/clean-build-tests/ch14
          --config $<CONFIG>
          --build-all
          --ctest ${CMAKE_CTEST_COMMAND}
          --run-ctest)
add_test(NAME ch14_checksum_mismatch_invalidates_benchmark
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_runtime_failure.py
          --compiler ${CMAKE_CXX_COMPILER}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch14/failures/checksum_mismatch.cpp
          --work-dir ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch14-checksum
          --expected-exit 2
          --diagnostic "benchmark-invalid checksum-mismatch")

add_test(NAME ch15_clean_concurrency_lab_configures_builds_and_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/test_clean_cmake_project.py
          --cmake ${CMAKE_COMMAND}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch15
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch15/expected/pipeline_compare.txt
          --target ch15_pipeline_compare
          --generator "${CMAKE_GENERATOR}"
          --compiler ${CMAKE_CXX_COMPILER}
          --work-root ${CMAKE_CURRENT_BINARY_DIR}/clean-build-tests/ch15
          --config $<CONFIG>
          --build-all
          --ctest ${CMAKE_CTEST_COMMAND}
          --run-ctest)

add_test(NAME ch16_clean_numerics_interop_lab_configures_builds_and_runs
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/test_clean_cmake_project.py
          --cmake ${CMAKE_COMMAND}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch16
          --expected ${CMAKE_CURRENT_SOURCE_DIR}/code/ch16/expected/stability_report.txt
          --target ch16_stability_report
          --generator "${CMAKE_GENERATOR}"
          --compiler ${CMAKE_CXX_COMPILER}
          --work-root ${CMAKE_CURRENT_BINARY_DIR}/clean-build-tests/ch16
          --config $<CONFIG>
          --build-all
          --ctest ${CMAKE_CTEST_COMMAND}
          --run-ctest)
add_test(NAME ch15_data_race_is_reported_by_thread_sanitizer
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_sanitizer_failure.py
          --compiler ${CMAKE_CXX_COMPILER}
          --source ${CMAKE_CURRENT_SOURCE_DIR}/code/ch15/failures/data_race.cpp
          --work-dir ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/ch15-race
          --sanitizer thread
          --diagnostic "ThreadSanitizer: data race")
set_tests_properties(ch15_data_race_is_reported_by_thread_sanitizer
  PROPERTIES
    RUN_SERIAL TRUE
    SKIP_RETURN_CODE 77)

if(NOT QUANT_BUILD_NEGATIVE_TESTS)
  include(${CMAKE_CURRENT_LIST_DIR}/QuantTesting.cmake)
  quant_disable_negative_tests(
    ch01_missing_semicolon_fails_during_compilation
    ch01_missing_definition_fails_during_linking
    ch01_startup_failure_is_observed_at_runtime
    ch02_narrowing_is_rejected_at_compile_time
    ch03_missing_definition_fails_during_linking
    ch03_duplicate_definition_fails_during_linking
    ch03_dangling_reference_emits_compile_diagnostic
    ch04_stale_iterator_is_reported_by_address_sanitizer
    ch05_private_state_is_rejected_at_compile_time
    ch06_use_after_free_is_reported_by_address_sanitizer
    ch07_dangling_view_is_reported_by_address_sanitizer
    ch08_wrong_override_fails_during_compilation
    ch09_unconstrained_error_exposes_missing_member
    ch09_concept_error_reports_unsatisfied_constraint
    ch11_missing_target_dependency_fails_during_linking
    ch12_out_of_bounds_is_reported_by_address_sanitizer
    ch12_dangling_access_is_reported_by_address_sanitizer
    ch12_signed_overflow_is_reported_by_undefined_sanitizer
    ch12_double_delete_is_reported_by_static_analyzer
    ch12_logic_bug_is_caught_by_behavior_oracle
    ch13_arena_dangling_is_reported_by_address_sanitizer
    ch14_checksum_mismatch_invalidates_benchmark
    ch15_data_race_is_reported_by_thread_sanitizer
    ch16_temporary_buffer_view_is_reported_by_address_sanitizer)
endif()
