cmake_minimum_required(VERSION 3.20)
project(ch07_ranges_pipeline LANGUAGES CXX)

add_executable(ch07_ranges_pipeline
  src/main.cpp
  src/csv_reader.cpp
  src/pipeline.cpp)
target_include_directories(ch07_ranges_pipeline PRIVATE include)
target_compile_features(ch07_ranges_pipeline PRIVATE cxx_std_20)

if(MSVC)
  target_compile_options(ch07_ranges_pipeline PRIVATE /W4 /permissive-)
else()
  target_compile_options(ch07_ranges_pipeline PRIVATE -Wall -Wextra -Wpedantic)
endif()
