cmake_minimum_required(VERSION 3.20)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(appendix_simd_demo LANGUAGES CXX)
  include(CTest)
endif()
include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/QuantTargetDefaults.cmake)

add_executable(appendix_simd_demo simd_factor.cpp)
quant_apply_target_defaults(appendix_simd_demo)

if(BUILD_TESTING)
  add_test(NAME appendix_simd_preserves_scalar_oracle COMMAND appendix_simd_demo)
  set_tests_properties(appendix_simd_preserves_scalar_oracle PROPERTIES
    PASS_REGULAR_EXPRESSION
      "simd-demo-ok elements=1027 width=[1-9][0-9]* checksum=1027")
endif()
