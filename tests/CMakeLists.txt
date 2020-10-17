# Set the project name
project (main_tests)

# add_library(${PROJECT_NAME} main_tests.cc main_tests.h)
# set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${PROJECT_NAME})

# target_include_directories(${PROJECT_NAME}
#     PUBLIC
#         ${PROJECT_SOURCE_DIR}
# )

option(ENABLE_UNIT_TESTS "Enable unit tests" ON)
message(STATUS "Enable testing: ${ENABLE_UNIT_TESTS}")

if(ENABLE_UNIT_TESTS)
  
  include(fetch_git_repo.cmake)

  fetch_git_repo(
    googletest
    ${CMAKE_BINARY_DIR}/_deps
    https://github.com/google/googletest.git
    release-1.10.0
  )

  # Prevent GoogleTest from overriding our compiler/linker options
  # when building with Visual Studio
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  # Prevent GoogleTest from using PThreads
  set(gtest_disable_pthreads ON CACHE BOOL "" FORCE)

  # adds the targers: gtest, gtest_main, gmock, gmock_main
  add_subdirectory(
    ${googletest_SOURCE_DIR}
    ${googletest_BINARY_DIR}
    )

  # Silence std::tr1 warning on MSVC
  if(MSVC)
    foreach(_tgt gtest gtest_main gmock gmock_main)
      target_compile_definitions(${_tgt}
        PRIVATE
          "_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING"
        )
    endforeach()
  endif()

  add_executable(cpp_test "")

  target_sources(cpp_test
    PRIVATE
      main_tests.cc
      main_tests.h
    )

  target_link_libraries(cpp_test
    PRIVATE
      gtest_main
      gmock_main
    )

  target_include_directories(cpp_test
    PUBLIC
    ${PROJECT_SOURCE_DIR}
  )

  enable_testing()

  add_test(
    NAME google_test
    COMMAND $<TARGET_FILE:cpp_test>
    )
endif()