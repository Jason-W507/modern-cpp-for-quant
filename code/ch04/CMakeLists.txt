cmake_minimum_required(VERSION 3.20)
project(ch04_csv_stats LANGUAGES CXX)

add_executable(ch04_csv_stats src/main.cpp src/csv_stats.cpp)
target_include_directories(ch04_csv_stats PRIVATE include)
target_compile_features(ch04_csv_stats PRIVATE cxx_std_20)

if(MSVC)
  target_compile_options(ch04_csv_stats PRIVATE /W4 /permissive-)
else()
  target_compile_options(ch04_csv_stats PRIVATE -Wall -Wextra -Wpedantic)
endif()
