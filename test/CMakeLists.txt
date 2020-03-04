add_library(crashpad_test OBJECT)
crashpad_set_props(crashpad_test)
target_link_libraries(crashpad_test PUBLIC gtest)

target_sources(crashpad_test
  PRIVATE
  errors.cc
  errors.h
  file.cc
  file.h
  filesystem.cc
  filesystem.h
  gtest_death.h
  hex_string.cc
  hex_string.h
  main_arguments.cc
  main_arguments.h
  multiprocess.h
  multiprocess_exec.cc
  multiprocess_exec.h
  process_type.cc
  process_type.h
  scoped_guarded_page.h
  scoped_module_handle.cc
  scoped_module_handle.h
  scoped_temp_dir.cc
  scoped_temp_dir.h
  test_paths.cc
  test_paths.h
)

if(UNIX)
  target_sources(crashpad_test
    PRIVATE
    scoped_guarded_page_posix.cc
    scoped_temp_dir_posix.cc
    multiprocess_exec_posix.cc
    multiprocess_posix.cc
  )
endif()

if(APPLE)
  target_sources(crashpad_test
    PRIVATE
    mac/dyld.cc
    mac/dyld.h
    mac/exception_swallower.cc
    mac/exception_swallower.h
    mac/mach_errors.cc
    mac/mach_errors.h
    mac/mach_multiprocess.cc
    mac/mach_multiprocess.h
  )
endif()

if(UNIX AND NOT APPLE)
  target_sources(crashpad_test
    PRIVATE
    linux/fake_ptrace_connection.cc
    linux/fake_ptrace_connection.h
    linux/get_tls.cc
    linux/get_tls.h
  )
endif()

if(WIN)
  target_sources(crashpad_test
    PRIVATE
    multiprocess_exec_win.cc
    scoped_guarded_page_win.cc
    scoped_temp_dir_win.cc
    win/child_launcher.cc
    win/child_launcher.h
    win/win_child_process.cc
    win/win_child_process.h
    win/win_multiprocess.cc
    win/win_multiprocess.h
    win/win_multiprocess_with_temp_dir.cc
    win/win_multiprocess_with_temp_dir.h
  )
endif()

add_executable(crashpad_test_test_multiprocess_exec_test_child)
crashpad_set_props(crashpad_test_test_multiprocess_exec_test_child)
target_sources(crashpad_test_test_multiprocess_exec_test_child PRIVATE multiprocess_exec_test_child.cc)
target_link_libraries(crashpad_test_test_multiprocess_exec_test_child PRIVATE mini_chromium_base gtest)
if(APPLE)
  target_link_libraries(crashpad_test_test_multiprocess_exec_test_child PRIVATE AppleFrameworks)
endif()


crashpad_add_test(crashpad_test_test)
target_sources(crashpad_test_test
  PRIVATE
  hex_string_test.cc
  main_arguments_test.cc
  multiprocess_exec_test.cc
  scoped_guarded_page_test.cc
  scoped_temp_dir_test.cc
  test_paths_test.cc
)

if(UNIX)
  target_sources(crashpad_test_test PRIVATE multiprocess_posix_test.cc)
endif()

if(APPLE)
  target_sources(crashpad_test_test PRIVATE mac/mach_multiprocess_test.cc)
endif()

target_link_libraries(crashpad_test_test
  PRIVATE
  gtest
  gtest_main
  gmock
  util
  mini_chromium_base
  crashpad_test
  crashpad_handler
  crashpad_client
  ZLIB::ZLIB
)

add_dependencies(crashpad_test_test crashpad_test_test_multiprocess_exec_test_child)

add_library(gtest_main OBJECT)
crashpad_set_props(gtest_main)
target_sources(gtest_main PRIVATE gtest_main.cc)
target_link_libraries(gtest_main PUBLIC gtest PRIVATE mini_chromium_base)
target_compile_definitions(gtest_main PUBLIC CRASHPAD_TEST_LAUNCHER_GTEST)

add_library(gmock_main OBJECT)
crashpad_set_props(gmock_main)
target_sources(gmock_main PRIVATE gtest_main.cc)
target_link_libraries(gmock_main PUBLIC gmock gtest PRIVATE mini_chromium_base)
target_compile_definitions(gmock_main PUBLIC CRASHPAD_TEST_LAUNCHER_GMOCK)

# This file is required for the tests to work - but
# the ctest launcher seems to run it from the subdirectory
# always - so let's make sure we copy it to two different places
file(
  COPY test_paths_test_data_root.txt
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test
)

file(
  COPY test_paths_test_data_root.txt
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)