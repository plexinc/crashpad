add_executable(crashpad_handler)
crashpad_set_props(crashpad_handler)
target_link_libraries(crashpad_handler
  PRIVATE
  crashpad_client
  ZLIB::ZLIB
  AppleFrameworks
)

set_target_properties(crashpad_handler
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_sources(crashpad_handler
  PRIVATE
  main.cc
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

