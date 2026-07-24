cmake_minimum_required(VERSION 3.20)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(appendix_interview_algorithms LANGUAGES CXX)
  include(CTest)
endif()
include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/QuantTargetDefaults.cmake)

add_executable(appendix_interview_algorithms interview_algorithms.cpp)
quant_apply_target_defaults(appendix_interview_algorithms)

if(BUILD_TESTING)
  add_test(NAME appendix_interview_algorithms_have_independent_oracles
    COMMAND appendix_interview_algorithms)
  set_tests_properties(appendix_interview_algorithms_have_independent_oracles
    PROPERTIES PASS_REGULAR_EXPRESSION
      "algorithms-ok top=9 lower-bound=3 lru-hit=10 window-max=8 mean=3.00 drawdown=0.25 merged=6")
endif()
