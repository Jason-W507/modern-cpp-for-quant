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
  set(PYBIND11_FINDPYTHON ON)
  find_package(Python3 COMPONENTS Interpreter Development.Module QUIET)
  find_package(pybind11 CONFIG QUIET)
  if(pybind11_FOUND)
    pybind11_add_module(quant_factor_kernel bindings/module.cpp)
    target_link_libraries(quant_factor_kernel PRIVATE capstone_factor_kernel)
  else()
    message(STATUS "factor kernel: pybind11 not found; building CLI and core only")
  endif()
endif()

option(FACTOR_KERNEL_BUILD_EIGEN "Build the pinned Eigen matrix kernel" OFF)
if(FACTOR_KERNEL_BUILD_EIGEN)
  include(FetchContent)
  FetchContent_Declare(eigen
    URL https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
  set(EIGEN_BUILD_DOC OFF CACHE BOOL "" FORCE)
  set(BUILD_TESTING_SAVED ${BUILD_TESTING})
  set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(eigen)
  set(BUILD_TESTING ${BUILD_TESTING_SAVED} CACHE BOOL "" FORCE)
  add_library(capstone_factor_kernel_eigen eigen/eigen_factor.cpp)
  target_link_libraries(capstone_factor_kernel_eigen
    PUBLIC capstone_factor_kernel Eigen3::Eigen)
  if(BUILD_TESTING)
    add_executable(capstone_factor_kernel_eigen_tests tests/test_eigen_factor.cpp)
    target_link_libraries(capstone_factor_kernel_eigen_tests
      PRIVATE capstone_factor_kernel_eigen)
    add_test(NAME capstone_factor_kernel_eigen_matches_scalar
      COMMAND capstone_factor_kernel_eigen_tests)
    set_tests_properties(capstone_factor_kernel_eigen_matches_scalar PROPERTIES
      PASS_REGULAR_EXPRESSION "eigen-factor-ok rows=4 scalar-match=1")
  endif()
  add_executable(capstone_factor_compare benchmarks/factor_compare.cpp)
  target_link_libraries(capstone_factor_compare
    PRIVATE capstone_factor_kernel_eigen)
endif()

option(FACTOR_KERNEL_BUILD_CATCH2 "Build the pinned Catch2 parameterized tests" OFF)
if(FACTOR_KERNEL_BUILD_CATCH2 AND BUILD_TESTING)
  include(FetchContent)
  FetchContent_Declare(Catch2
    URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.15.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
  FetchContent_MakeAvailable(Catch2)
  add_executable(capstone_factor_kernel_catch2_tests
    tests/test_factor_kernel_catch2.cpp)
  target_link_libraries(capstone_factor_kernel_catch2_tests
    PRIVATE capstone_factor_kernel Catch2::Catch2WithMain)
  add_test(NAME capstone_factor_kernel_catch2_parameterized
    COMMAND capstone_factor_kernel_catch2_tests)
endif()

if(BUILD_TESTING)
  add_executable(capstone_factor_kernel_tests tests/test_factor_kernel.cpp)
  target_link_libraries(capstone_factor_kernel_tests PRIVATE capstone_factor_kernel)
  add_test(NAME capstone_factor_kernel_matches_worked_batch
    COMMAND capstone_factor_kernel_tests)
  set_tests_properties(capstone_factor_kernel_matches_worked_batch PROPERTIES
    PASS_REGULAR_EXPRESSION
      "factor-kernel-tests-ok rows=4 finite=3 missing=1 shape-overflow-rejected=1")

  find_package(Python3 REQUIRED COMPONENTS Interpreter)
  add_test(NAME capstone_factor_report_matches_generated_pair_statistics
    COMMAND ${Python3_EXECUTABLE}
      ${CMAKE_CURRENT_LIST_DIR}/tests/test_factor_report.py
      ${CMAKE_CURRENT_LIST_DIR}/reports/factor-eigen-comparison.json
      ${CMAKE_CURRENT_LIST_DIR}/reports/OPTIMIZATION.md
      ${CMAKE_CURRENT_LIST_DIR}/../../appendices/capstones.tex)
  set_tests_properties(capstone_factor_report_matches_generated_pair_statistics
    PROPERTIES PASS_REGULAR_EXPRESSION
      "factor-report-contract-ok generated-medians=1 manuscript-match=1")
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

  if(TARGET quant_factor_kernel)
    execute_process(COMMAND ${Python3_EXECUTABLE} -c "import numpy, pyarrow"
      RESULT_VARIABLE FACTOR_KERNEL_ARROW_RESULT
      OUTPUT_QUIET ERROR_QUIET)
    if(FACTOR_KERNEL_ARROW_RESULT EQUAL 0)
      get_filename_component(FACTOR_KERNEL_CXX_RUNTIME_DIR
        "${CMAKE_CXX_COMPILER}" DIRECTORY)
      add_test(NAME capstone_factor_kernel_pybind_matches_arrow_numpy
        COMMAND ${CMAKE_COMMAND} -E env
          "PYTHONPATH=$<TARGET_FILE_DIR:quant_factor_kernel>"
          "FACTOR_KERNEL_DLL_DIR=${FACTOR_KERNEL_CXX_RUNTIME_DIR}"
          ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/python/test_binding_consistency.py)
      set_tests_properties(capstone_factor_kernel_pybind_matches_arrow_numpy
        PROPERTIES PASS_REGULAR_EXPRESSION
          "pybind-arrow-consistency-ok rows=4 dtype=float64 contiguous=1 gil-released=1")
    endif()
  endif()
endif()
