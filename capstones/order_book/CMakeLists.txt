cmake_minimum_required(VERSION 3.20)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(quant_order_book_capstone LANGUAGES CXX)
endif()

include(CTest)
find_package(Threads REQUIRED)

add_library(capstone_order_book INTERFACE)
target_include_directories(capstone_order_book INTERFACE include)
target_compile_features(capstone_order_book INTERFACE cxx_std_20)

add_executable(capstone_order_book_demo src/main.cpp)
target_link_libraries(capstone_order_book_demo PRIVATE capstone_order_book)

add_library(capstone_market_replay src/market_replay.cpp)
target_link_libraries(capstone_market_replay PUBLIC capstone_order_book)
target_include_directories(capstone_market_replay PUBLIC include)

add_executable(capstone_replay_benchmark benchmarks/replay_benchmark.cpp)
target_link_libraries(capstone_replay_benchmark PRIVATE capstone_market_replay)

option(CAPSTONE_BUILD_FUZZERS "Build libFuzzer protocol targets with Clang" OFF)
if(CAPSTONE_BUILD_FUZZERS)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "CAPSTONE_BUILD_FUZZERS requires Clang/libFuzzer")
  endif()
  add_executable(capstone_market_replay_fuzzer fuzz/market_replay_fuzz.cpp)
  target_link_libraries(capstone_market_replay_fuzzer PRIVATE capstone_market_replay)
  target_compile_options(capstone_market_replay_fuzzer PRIVATE
    -fsanitize=fuzzer,address,undefined -fno-omit-frame-pointer)
  target_link_options(capstone_market_replay_fuzzer PRIVATE
    -fsanitize=fuzzer,address,undefined)
endif()

if(MSVC)
  target_compile_options(capstone_order_book_demo PRIVATE /W4 /permissive-)
else()
  target_compile_options(capstone_order_book_demo PRIVATE
    -Wall -Wextra -Wpedantic)
endif()

if(BUILD_TESTING)
  add_executable(capstone_order_book_tests tests/test_order_book.cpp)
  target_link_libraries(capstone_order_book_tests PRIVATE capstone_order_book)
  if(MSVC)
    target_compile_options(capstone_order_book_tests PRIVATE /W4 /permissive-)
  else()
    target_compile_options(capstone_order_book_tests PRIVATE
      -Wall -Wextra -Wpedantic)
  endif()
  add_test(NAME capstone_order_book_preserves_price_time_priority
    COMMAND capstone_order_book_tests)
  set_tests_properties(capstone_order_book_preserves_price_time_priority
    PROPERTIES PASS_REGULAR_EXPRESSION
      "order-book-tests-ok trades=1 best-bid=100 best-ask=101 replay-next=3")

  add_executable(capstone_market_replay_tests tests/test_market_replay.cpp)
  target_link_libraries(capstone_market_replay_tests PRIVATE capstone_market_replay)
  add_test(NAME capstone_market_replay_classifies_sequence_failures
    COMMAND capstone_market_replay_tests)
  set_tests_properties(capstone_market_replay_classifies_sequence_failures
    PROPERTIES PASS_REGULAR_EXPRESSION
      "market-replay-tests-ok sequence-accepted=4 book-accepted=4 duplicate=1 gap=1 buffered=1 recovered=1 decode=1 trades=2 bounded=1 explicit-outcome=1 next=5")

  add_test(NAME capstone_replay_benchmark_preserves_message_count
    COMMAND capstone_replay_benchmark)
  set_tests_properties(capstone_replay_benchmark_preserves_message_count
    PROPERTIES PASS_REGULAR_EXPRESSION
      "replay-benchmark-ok messages=10000 sequence-accepted=10000 book-accepted=10000")

  find_package(Python3 COMPONENTS Interpreter QUIET)
  if(Python3_Interpreter_FOUND)
  add_test(NAME capstone_replay_multiprocess_report_is_reproducible
      COMMAND ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_LIST_DIR}/benchmarks/run_replay_benchmark.py
        $<TARGET_FILE:capstone_replay_benchmark>
        --output ${CMAKE_CURRENT_BINARY_DIR}/replay-multiprocess-test.json
        --processes 3)
  set_tests_properties(capstone_replay_multiprocess_report_is_reproducible
    PROPERTIES PASS_REGULAR_EXPRESSION
      "replay-multiprocess-ok processes=3 samples=300")
  add_test(NAME capstone_replay_report_uses_nearest_rank_without_process_p999
    COMMAND ${Python3_EXECUTABLE}
      ${CMAKE_CURRENT_LIST_DIR}/tests/test_replay_report.py
      ${CMAKE_CURRENT_LIST_DIR}/reports/replay-benchmark.json)
  set_tests_properties(capstone_replay_report_uses_nearest_rank_without_process_p999
    PROPERTIES PASS_REGULAR_EXPRESSION
      "replay-report-contract-ok nearest-rank=1 aggregate-p999=1")
endif()

  add_executable(capstone_spsc_replay_tests tests/test_spsc_replay.cpp)
  target_link_libraries(capstone_spsc_replay_tests
    PRIVATE capstone_market_replay Threads::Threads)
  add_test(NAME capstone_spsc_replay_preserves_every_message
    COMMAND capstone_spsc_replay_tests)
  set_tests_properties(capstone_spsc_replay_preserves_every_message
    PROPERTIES PASS_REGULAR_EXPRESSION
      "spsc-replay-ok offered=10000 sequence-accepted=10000 book-accepted=10000 next=10001")
endif()
