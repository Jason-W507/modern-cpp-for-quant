add_test(NAME book_contract
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/check_book_contract.py
          --root ${CMAKE_CURRENT_SOURCE_DIR})
set_tests_properties(book_contract PROPERTIES
  PASS_REGULAR_EXPRESSION "18 chapters.*235 planned pages")

add_test(NAME book_contract_rejects_invalid_authoring_evidence
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/tests/test_book_contract.py)

add_test(NAME output_harness_rejects_whitespace_drift
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/tests/test_output_contract.py)

add_test(NAME p2_editorial_contract
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/tests/test_p2_editorial_contract.py)

add_test(NAME ci_workflow_contract
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/tests/test_ci_contract.py)

add_test(NAME pdf_build_log_contract
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/tests/test_pdf_build_check.py)

add_test(NAME version_contract_has_one_source
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/tests/test_version_contract.py)

add_test(NAME public_repository_contract
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/tests/test_public_repository_contract.py)

add_test(NAME cmake_architecture_contract
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/tests/test_cmake_architecture.py)

add_test(NAME release_package_contract
  COMMAND ${Python3_EXECUTABLE}
          ${CMAKE_CURRENT_SOURCE_DIR}/tools/tests/test_release_package.py)

