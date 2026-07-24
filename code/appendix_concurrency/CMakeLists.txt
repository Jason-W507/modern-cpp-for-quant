cmake_minimum_required(VERSION 3.20)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(appendix_spsc_demo LANGUAGES CXX)
  include(CTest)
endif()
include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/QuantTargetDefaults.cmake)
find_package(Threads REQUIRED)

add_executable(appendix_spsc_demo spsc_demo.cpp)
quant_apply_target_defaults(appendix_spsc_demo)
target_link_libraries(appendix_spsc_demo PRIVATE Threads::Threads)

if(BUILD_TESTING)
  add_test(NAME appendix_spsc_preserves_every_item COMMAND appendix_spsc_demo)
  set_tests_properties(appendix_spsc_preserves_every_item PROPERTIES
    PASS_REGULAR_EXPRESSION
      "spsc-ok produced=10000 consumed=10000 checksum=50005000")
endif()
