cmake_minimum_required(VERSION 3.20)
project(ch03_market_tool_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ch03_market_tool src/main.cpp src/market_math.cpp)
target_include_directories(ch03_market_tool PRIVATE include)

add_executable(ch03_parameter_modes parameter_modes.cpp)
add_executable(ch03_basis_points_solution exercises/basis_points_solution.cpp)
