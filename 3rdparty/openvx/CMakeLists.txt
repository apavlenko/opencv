if(NOT HAVE_OPENVX)
  message(STATUS "OpenVX is not available, disabling openvx-related HAL and stuff")
  return()
endif()

set(OPENCV_3P_OPENVX_DIR ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(hal)


# ivx tests
if(0)
set(ivx_test_path "${CMAKE_CURRENT_LIST_DIR}/test")
if(BUILD_TESTS AND EXISTS "${ivx_test_path}")
  set(ivx_test_deps opencv_core opencv_ts)
  #ocv_check_dependencies(${ivx_test_deps})
  #if(OCV_DEPENDENCIES_FOUND)
    set(the_target "opencv_test_ivx")
    file(GLOB_RECURSE ivx_test_srcs "${ivx_test_path}/*.cpp")
    file(GLOB_RECURSE ivx_test_hdrs "${ivx_test_path}/*.hpp" "${test_path}/*.h")
    ocv_source_group("Src" DIRBASE "${ivx_test_path}" FILES ${ivx_test_srcs})
    ocv_source_group("Include" DIRBASE "${ivx_test_path}" FILES ${ivx_test_hdrs})
    ocv_add_executable(${the_target} ${ivx_test_srcs} ${ivx_test_hdrs})
    ocv_target_include_modules(${the_target} ${ivx_test_deps} "${ivx_test_path}")
    ocv_target_link_libraries(${the_target} ${ivx_test_deps})
    #add_dependencies(opencv_tests ${the_target})
    set_target_properties(${the_target} PROPERTIES
      DEBUG_POSTFIX "${OPENCV_DEBUG_POSTFIX}"
      RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
    )
    if(ENABLE_SOLUTION_FOLDERS)
      set_target_properties(${the_target} PROPERTIES FOLDER "tests accuracy")
    endif()
  #endif(OCV_DEPENDENCIES_FOUND)

  if(INSTALL_TESTS)
    install(TARGETS ${the_target} RUNTIME DESTINATION ${OPENCV_TEST_INSTALL_PATH} COMPONENT tests)
  endif()
endif()
endif(0)
