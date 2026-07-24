
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
