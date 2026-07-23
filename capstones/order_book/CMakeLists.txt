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
endif()
