# taken from http://johnlamp.net/cmake-tutorial-5-functionally-improved-testing.html
function(add_gmock_test target)
    add_executable(${target} ${ARGN})
    target_link_libraries(${target} gtest gtest_main ${CMAKE_THREAD_LIBS_INIT})

    set_property(TARGET ${target} PROPERTY CXX_STANDARD 17)
    set_property(TARGET ${target} PROPERTY CXX_STANDARD_REQUIRED ON)

    add_test(${target} ${target})

    # Skip post-build test execution on Windows when building shared libs
    # because DLLs are not in the executable's directory.
    # Use ctest to run tests instead.
    if(NOT (WIN32 AND BUILD_SHARED_LIBS))
      add_custom_command(TARGET ${target}
                         POST_BUILD
                         COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>_failed
                         COMMAND ${target}
                         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                         COMMENT "Running ${target}" VERBATIM)

      add_custom_command(TARGET ${target}
                         POST_BUILD
                         COMMAND ${CMAKE_COMMAND} -E remove $<TARGET_FILE:${target}>_failed
                         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                         COMMENT "Cleanup ${target}" VERBATIM)
    endif()
endfunction()
