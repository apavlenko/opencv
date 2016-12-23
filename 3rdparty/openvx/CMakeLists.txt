if(NOT HAVE_OPENVX)
  message(STATUS "OpenVX is not available, disabling openvx-related HAL and stuff")
  return()
endif()

set(OPENCV_3P_OPENVX_DIR ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(hal)


# ivx tests
if(BUILD_opencv_ts)
  set(the_target "opencv_test_ivx")
  set(the_srcs_path "${CMAKE_CURRENT_LIST_DIR}/test")
  file(GLOB_RECURSE the_srcs "${the_srcs_path}/*.cpp" "${the_srcs_path}/*.hpp")
  add_executable(${the_target} ${the_srcs})
  #set_target_properties(${the_target} PROPERTIES LINKER_LANGUAGE CXX)
  target_include_directories(${the_target} PUBLIC "${OPENCV_MODULE_opencv_ts_LOCATION}/include")
  target_link_libraries(${the_target} opencv_ts)
endif(BUILD_opencv_ts)
