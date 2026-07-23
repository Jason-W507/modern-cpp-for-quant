cmake_minimum_required(VERSION 3.20)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(quant_order_book_capstone LANGUAGES CXX)
endif()

include(CTest)

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
      "market-replay-tests-ok accepted=3 duplicate=1 gap=1 decode=1 trades=1 next=4")

  add_test(NAME capstone_replay_benchmark_preserves_message_count
    COMMAND capstone_replay_benchmark)
  set_tests_properties(capstone_replay_benchmark_preserves_message_count
    PROPERTIES PASS_REGULAR_EXPRESSION
      "replay-benchmark-ok messages=10000 accepted=10000")
endif()
