cmake_minimum_required(VERSION 3.20)
project(ch05_market_analyzer LANGUAGES CXX)

add_executable(ch05_market_analyzer
  src/main.cpp
  src/csv_reader.cpp
  src/market.cpp)
target_include_directories(ch05_market_analyzer PRIVATE include)
target_compile_features(ch05_market_analyzer PRIVATE cxx_std_20)

if(MSVC)
  target_compile_options(ch05_market_analyzer PRIVATE /W4 /permissive-)
else()
  target_compile_options(ch05_market_analyzer PRIVATE -Wall -Wextra -Wpedantic)
endif()
