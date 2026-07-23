cmake_minimum_required(VERSION 3.20)
project(ch11_cmake_targets LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR AND
   NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

option(CH11_ENABLE_SANITIZERS "Enable AddressSanitizer and UBSan" OFF)

include(CTest)

add_library(ch11_project_options INTERFACE)
target_compile_features(ch11_project_options INTERFACE cxx_std_20)
if(MSVC)
  target_compile_options(ch11_project_options INTERFACE /W4 /permissive-)
else()
  target_compile_options(ch11_project_options INTERFACE
    -Wall -Wextra -Wpedantic)
  if(CH11_ENABLE_SANITIZERS)
    target_compile_options(ch11_project_options INTERFACE
      -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(ch11_project_options INTERFACE
      -fsanitize=address,undefined)
  endif()
endif()

add_library(ch11_market STATIC src/market_summary.cpp)
target_include_directories(ch11_market PUBLIC include)
target_link_libraries(ch11_market PUBLIC ch11_project_options)
target_compile_definitions(ch11_market PRIVATE CH11_BUILDING_LIBRARY=1)

add_executable(ch11_market_report app/main.cpp)
target_link_libraries(ch11_market_report PRIVATE ch11_market)

add_executable(ch11_market_example examples/market_example.cpp)
target_link_libraries(ch11_market_example PRIVATE ch11_market)

add_executable(ch11_notional_solution exercises/notional_report_solution.cpp)
target_link_libraries(ch11_notional_solution PRIVATE ch11_market)

add_executable(ch11_missing_target_link_failure EXCLUDE_FROM_ALL
  failures/missing_target_link.cpp)
target_link_libraries(ch11_missing_target_link_failure PRIVATE
  ch11_project_options)

if(BUILD_TESTING)
  add_executable(ch11_market_tests tests/test_market_summary.cpp)
  target_link_libraries(ch11_market_tests PRIVATE ch11_market)
  add_test(NAME ch11_market_summary_behavior COMMAND ch11_market_tests)

  add_test(NAME ch11_market_report_runs COMMAND ch11_market_report)
  set_tests_properties(ch11_market_report_runs PROPERTIES
    PASS_REGULAR_EXPRESSION
      "rows=3 quantity=25 notional=2525.00 vwap=101.00")
  add_test(NAME ch11_market_example_runs COMMAND ch11_market_example)
  set_tests_properties(ch11_market_example_runs PROPERTIES
    PASS_REGULAR_EXPRESSION "example-quantity=4 example-vwap=100")
  add_test(NAME ch11_notional_solution_runs COMMAND ch11_notional_solution)
  set_tests_properties(ch11_notional_solution_runs PROPERTIES
    PASS_REGULAR_EXPRESSION "exercise-notional=200.00")
endif()
