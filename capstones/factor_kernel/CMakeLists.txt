cmake_minimum_required(VERSION 3.20)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(quant_factor_kernel_capstone LANGUAGES CXX)
endif()

include(CTest)

add_library(capstone_factor_kernel src/factor_kernel.cpp)
target_include_directories(capstone_factor_kernel PUBLIC include)
target_compile_features(capstone_factor_kernel PUBLIC cxx_std_20)

if(MSVC)
  target_compile_options(capstone_factor_kernel PRIVATE /W4 /permissive-)
else()
  target_compile_options(capstone_factor_kernel PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(capstone_factor_kernel_cli src/main.cpp)
target_link_libraries(capstone_factor_kernel_cli PRIVATE capstone_factor_kernel)

option(FACTOR_KERNEL_BUILD_PYTHON "Build the optional pybind11 batch module" ON)
if(FACTOR_KERNEL_BUILD_PYTHON)
  find_package(pybind11 CONFIG QUIET)
  if(pybind11_FOUND)
    pybind11_add_module(quant_factor_kernel bindings/module.cpp)
    target_link_libraries(quant_factor_kernel PRIVATE capstone_factor_kernel)
  else()
    message(STATUS "factor kernel: pybind11 not found; building CLI and core only")
  endif()
endif()

if(BUILD_TESTING)
  add_executable(capstone_factor_kernel_tests tests/test_factor_kernel.cpp)
  target_link_libraries(capstone_factor_kernel_tests PRIVATE capstone_factor_kernel)
  add_test(NAME capstone_factor_kernel_matches_worked_batch
    COMMAND capstone_factor_kernel_tests)
  set_tests_properties(capstone_factor_kernel_matches_worked_batch PROPERTIES
    PASS_REGULAR_EXPRESSION
      "factor-kernel-tests-ok rows=4 finite=3 missing=1")

  find_package(Python3 REQUIRED COMPONENTS Interpreter)
  execute_process(COMMAND ${Python3_EXECUTABLE} -c "import numpy"
    RESULT_VARIABLE FACTOR_KERNEL_NUMPY_RESULT
    OUTPUT_QUIET ERROR_QUIET)
  if(FACTOR_KERNEL_NUMPY_RESULT EQUAL 0)
    add_test(NAME capstone_factor_kernel_matches_numpy_reference
      COMMAND ${Python3_EXECUTABLE}
              ${CMAKE_CURRENT_LIST_DIR}/python/test_numpy_consistency.py
              $<TARGET_FILE:capstone_factor_kernel_cli>
              ${CMAKE_CURRENT_BINARY_DIR}/numpy-test)
  endif()
endif()
