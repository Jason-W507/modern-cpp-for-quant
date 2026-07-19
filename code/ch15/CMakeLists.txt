cmake_minimum_required(VERSION 3.20)
project(ch15_concurrency_lab LANGUAGES CXX)

include(CTest)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_package(Threads REQUIRED)

add_library(ch15_project_options INTERFACE)
target_compile_features(ch15_project_options INTERFACE cxx_std_20)
if(MSVC)
  target_compile_options(ch15_project_options INTERFACE /W4 /permissive-)
else()
  target_compile_options(ch15_project_options INTERFACE
    -Wall -Wextra -Wpedantic)
endif()

add_executable(ch15_pipeline_compare experiments/pipeline_compare.cpp)
target_link_libraries(ch15_pipeline_compare
  PRIVATE ch15_project_options Threads::Threads)

add_executable(ch15_bounded_queue_behavior tests/bounded_queue_behavior.cpp)
target_include_directories(ch15_bounded_queue_behavior PRIVATE include)
target_link_libraries(ch15_bounded_queue_behavior
  PRIVATE ch15_project_options Threads::Threads)

add_executable(ch15_protocol_demo experiments/protocol_demo.cpp)
target_include_directories(ch15_protocol_demo PRIVATE include)
target_link_libraries(ch15_protocol_demo
  PRIVATE ch15_project_options Threads::Threads)

add_executable(ch15_data_race_failure EXCLUDE_FROM_ALL
  failures/data_race.cpp)
target_link_libraries(ch15_data_race_failure
  PRIVATE ch15_project_options Threads::Threads)

add_executable(ch15_drop_newest_solution
  exercises/drop_newest_solution.cpp)
target_include_directories(ch15_drop_newest_solution PRIVATE include)
target_link_libraries(ch15_drop_newest_solution
  PRIVATE ch15_project_options Threads::Threads)

add_test(NAME ch15_sequential_threads_and_tasks_preserve_results
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../../tools/expect_output.py
          --expected ${CMAKE_CURRENT_LIST_DIR}/expected/pipeline_compare.txt
          -- $<TARGET_FILE:ch15_pipeline_compare>)

add_test(NAME ch15_bounded_queue_reports_full_and_drains_before_closed
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../../tools/expect_output.py
          --expected ${CMAKE_CURRENT_LIST_DIR}/expected/bounded_queue_behavior.txt
          -- $<TARGET_FILE:ch15_bounded_queue_behavior>)

add_test(NAME ch15_start_stop_and_exception_protocol_is_observable
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../../tools/expect_output.py
          --expected ${CMAKE_CURRENT_LIST_DIR}/expected/protocol_demo.txt
          -- $<TARGET_FILE:ch15_protocol_demo>)

add_test(NAME ch15_drop_newest_solution_reports_every_offered_item
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../../tools/expect_output.py
          --expected ${CMAKE_CURRENT_LIST_DIR}/expected/drop_newest_solution.txt
          -- $<TARGET_FILE:ch15_drop_newest_solution>)
