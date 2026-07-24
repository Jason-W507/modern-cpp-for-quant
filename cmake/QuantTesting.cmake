include_guard(GLOBAL)

function(quant_disable_negative_tests)
  foreach(test_name IN LISTS ARGN)
    if(TEST ${test_name})
      set_tests_properties(${test_name} PROPERTIES DISABLED TRUE)
    endif()
  endforeach()
endfunction()
