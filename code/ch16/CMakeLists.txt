cmake_minimum_required(VERSION 3.20)
project(ch16_numerics_interop_lab LANGUAGES CXX)

include(CTest)
option(CH16_BUILD_PYTHON "Build the optional pybind11 module when available" ON)
if(CH16_BUILD_PYTHON)
  find_package(Python3 REQUIRED COMPONENTS Interpreter
    OPTIONAL_COMPONENTS Development.Module)
else()
  find_package(Python3 REQUIRED COMPONENTS Interpreter)
endif()

add_library(ch16_project_options INTERFACE)
target_compile_features(ch16_project_options INTERFACE cxx_std_20)
if(MSVC)
  target_compile_options(ch16_project_options INTERFACE /W4 /permissive-)
else()
  target_compile_options(ch16_project_options INTERFACE
    -Wall -Wextra -Wpedantic)
endif()

add_library(ch16_numerics src/numerics.cpp)
target_include_directories(ch16_numerics PUBLIC include)
target_link_libraries(ch16_numerics PUBLIC ch16_project_options)
set_target_properties(ch16_numerics PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_executable(ch16_stability_report experiments/stability_report.cpp)
target_link_libraries(ch16_stability_report PRIVATE ch16_numerics)

add_executable(ch16_finance_report experiments/finance_report.cpp)
target_link_libraries(ch16_finance_report PRIVATE ch16_numerics)

add_executable(ch16_boundary_report experiments/boundary_report.cpp)
target_link_libraries(ch16_boundary_report PRIVATE ch16_numerics)

add_executable(ch16_return_statistics_solution
  exercises/return_statistics_solution.cpp)
target_link_libraries(ch16_return_statistics_solution PRIVATE ch16_numerics)

add_test(NAME ch16_stable_reduction_matches_independent_oracle
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../../tools/expect_output.py
          --expected ${CMAKE_CURRENT_LIST_DIR}/expected/stability_report.txt
          -- $<TARGET_FILE:ch16_stability_report>)

add_test(NAME ch16_finance_values_state_representation_and_rounding
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../../tools/expect_output.py
          --expected ${CMAKE_CURRENT_LIST_DIR}/expected/finance_report.txt
          -- $<TARGET_FILE:ch16_finance_report>)

add_test(NAME ch16_batch_boundary_exposes_cost_and_validation
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../../tools/expect_output.py
          --expected ${CMAKE_CURRENT_LIST_DIR}/expected/boundary_report.txt
          -- $<TARGET_FILE:ch16_boundary_report>)

add_test(NAME ch16_return_statistics_solution_matches_worked_example
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../../tools/expect_output.py
          --expected ${CMAKE_CURRENT_LIST_DIR}/expected/return_statistics_solution.txt
          -- $<TARGET_FILE:ch16_return_statistics_solution>)

add_test(NAME ch16_temporary_buffer_view_is_reported_by_address_sanitizer
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_LIST_DIR}/../../tools/expect_sanitizer_failure.py
          --compiler ${CMAKE_CXX_COMPILER}
          --source ${CMAKE_CURRENT_LIST_DIR}/failures/temporary_buffer_view.cpp
          --work-dir ${CMAKE_CURRENT_BINARY_DIR}/intentional-failures/temporary-view
          --sanitizer address
          --diagnostic "heap-use-after-free")
set_tests_properties(
  ch16_temporary_buffer_view_is_reported_by_address_sanitizer
  PROPERTIES SKIP_RETURN_CODE 77)

# CPython's official Windows ABI is built with MSVC.  A MinGW extension may
# compile successfully yet fail to load because its runtime/ABI does not match.
if(CH16_BUILD_PYTHON AND Python3_Development.Module_FOUND AND NOT MINGW)
  find_package(Threads REQUIRED)
  Python3_add_library(ch16_gil_probe MODULE WITH_SOABI failures/gil_probe.cpp)
  set_target_properties(ch16_gil_probe PROPERTIES
    OUTPUT_NAME "_quant_ch16_gil_probe")
  target_link_libraries(ch16_gil_probe PRIVATE
    ch16_project_options
    Threads::Threads)
  # The probe is loaded by the system CPython executable, which is not linked
  # to the sanitizer runtime. Keep this functional GIL test uninstrumented in
  # an otherwise sanitized build so the extension remains loadable.
  if(CMAKE_CXX_FLAGS MATCHES "(^|[ ;])-fsanitize=")
    target_compile_options(ch16_gil_probe PRIVATE -fno-sanitize=all)
    target_link_options(ch16_gil_probe PRIVATE -fno-sanitize=all)
  endif()
  add_test(NAME ch16_gil_probe_distinguishes_held_and_released_progress
    COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONPATH=$<TARGET_FILE_DIR:ch16_gil_probe>"
            ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_LIST_DIR}/../../tools/expect_output.py
            --expected ${CMAKE_CURRENT_LIST_DIR}/expected/gil_probe.txt
            -- ${Python3_EXECUTABLE}
               ${CMAKE_CURRENT_LIST_DIR}/python/test_gil_probe.py)
endif()

if(CH16_BUILD_PYTHON)
  find_package(pybind11 CONFIG QUIET)
  if(pybind11_FOUND)
    pybind11_add_module(quant_ch16 bindings/module.cpp)
    target_link_libraries(quant_ch16 PRIVATE ch16_numerics)
    execute_process(
      COMMAND ${Python3_EXECUTABLE} -c "import numpy"
      RESULT_VARIABLE CH16_NUMPY_IMPORT_RESULT
      OUTPUT_QUIET ERROR_QUIET)
    if(CH16_NUMPY_IMPORT_RESULT EQUAL 0)
      get_filename_component(CH16_CXX_RUNTIME_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
      add_test(NAME ch16_optional_pybind_module_preserves_core_behavior
        COMMAND ${CMAKE_COMMAND} -E env
                "PYTHONPATH=$<TARGET_FILE_DIR:quant_ch16>"
                "QUANT_CH16_DLL_DIR=${CH16_CXX_RUNTIME_DIR}"
                ${Python3_EXECUTABLE}
                ${CMAKE_CURRENT_LIST_DIR}/python/test_optional_module.py)
    endif()
  else()
    message(STATUS "ch16: pybind11 not found; building dependency-free core only")
  endif()
endif()
