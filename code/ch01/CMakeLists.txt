cmake_minimum_required(VERSION 3.20)
project(ch01_first_build LANGUAGES CXX)
include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/QuantTargetDefaults.cmake)

add_executable(ch01_first_program first_program.cpp)
quant_apply_target_defaults(ch01_first_program)
