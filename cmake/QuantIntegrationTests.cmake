include_guard(GLOBAL)

if(NOT BUILD_TESTING)
  return()
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter)


include(${CMAKE_CURRENT_LIST_DIR}/QuantAuthoringTests.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/QuantChapterIntegrationTests.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/QuantAdvancedIntegrationTests.cmake)
