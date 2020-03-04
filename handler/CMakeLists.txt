add_library(crashpad_handler OBJECT)
crashpad_set_props(crashpad_handler)

target_sources(crashpad_handler
  PRIVATE
  crash_report_upload_thread.cc
  crash_report_upload_thread.h
  handler_main.cc
  handler_main.h
  minidump_to_upload_parameters.cc
  minidump_to_upload_parameters.h
  prune_crash_reports_thread.cc
  prune_crash_reports_thread.h
  user_stream_data_source.cc
  user_stream_data_source.h
)

if(UNIX AND NOT APPLE)
  target_sources(crashpad_handler
    PRIVATE
    linux/capture_snapshot.cc
    linux/capture_snapshot.h
    linux/crash_report_exception_handler.cc
    linux/crash_report_exception_handler.h
    linux/exception_handler_server.cc
    linux/exception_handler_server.h
    linux/cros_crash_report_exception_handler.cc
    linux/cros_crash_report_exception_handler.h
  )
endif()

if(APPLE)
  target_sources(crashpad_handler
    PRIVATE
    mac/crash_report_exception_handler.cc
    mac/crash_report_exception_handler.h
    mac/exception_handler_server.cc
    mac/exception_handler_server.h
    mac/file_limit_annotation.cc
    mac/file_limit_annotation.h
  )
endif()

if(WIN)
  target_sources(crashpad_handler
    PRIVATE
    win/crash_report_exception_handler.cc
    win/crash_report_exception_handler.h
  )
endif()

add_executable(crashpad_handler_exe main.cc)
crashpad_set_props(crashpad_handler_exe)

set_target_properties(crashpad_handler_exe
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
  OUTPUT_NAME crashpad_handler
)

target_link_libraries(crashpad_handler_exe
  PRIVATE
  crashpad_client
  ZLIB::ZLIB
  AppleFrameworks
  crashpad_handler
)

add_executable(crashpad_handler_test_extended_handler)
crashpad_set_props(crashpad_handler_test_extended_handler)
target_sources(crashpad_handler_test_extended_handler PRIVATE crashpad_handler_test_extended_handler.cc)
target_link_libraries(crashpad_handler_test_extended_handler
  PRIVATE
  gtest
  crashpad_test
  minidump
  minidump_test_support
  snapshot
  snapshot_test_support
  util
  crashpad_handler
  mini_chromium_base
  AppleFrameworks
  tool_support
  crashpad_client
  ZLIB::ZLIB
)

crashpad_add_test(crashpad_handler_test)
target_sources(crashpad_handler_test PRIVATE minidump_to_upload_parameters_test.cc)
target_link_libraries(crashpad_handler_test
  PRIVATE
  gtest
  gtest_main
  crashpad_test
  crashpad_client
  snapshot
  snapshot_test_support
  util
  crashpad_handler
  mini_chromium_base
  ZLIB::ZLIB
)

add_dependencies(crashpad_handler_test crashpad_handler_test_extended_handler)