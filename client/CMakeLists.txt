add_library(crashpad_client STATIC)
crashpad_set_props(crashpad_client)
target_link_libraries(crashpad_client
  PRIVATE
  minidump
  snapshot
  mini_chromium_base
  tool_support
  util
)

set_target_properties(crashpad_client
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_sources(crashpad_client
  PRIVATE
  annotation.cc
  annotation.h
  annotation_list.cc
  annotation_list.h
  crash_report_database.cc
  crash_report_database.h
  crashpad_client.h
  crashpad_info.cc
  crashpad_info.h
  prune_crash_reports.cc
  prune_crash_reports.h
  settings.cc
  settings.h
  simple_address_range_bag.h
  simple_string_dictionary.h
  simulate_crash.h
)

if(APPLE)
  target_sources(crashpad_client
    PRIVATE
    crash_report_database_mac.mm
    crashpad_client_mac.cc
    simulate_crash_mac.cc
    simulate_crash_mac.h
  )
endif()

if(UNIX AND NOT APPLE)
  target_sources(crashpad_client
    PRIVATE
    crashpad_client_linux.cc
    simulate_crash_linux.h
    client_argv_handling.cc
    client_argv_handling.h
    crashpad_info_note.S
    crash_report_database_generic.cc
  )
endif()

if(WIN32)
  target_sources(crashpad_client
    PRIVATE
    crash_report_database_win.cc
    crashpad_client_win.cc
    simulate_crash_win.h
  )
endif()

crashpad_add_test(crashpad_client_test)

target_sources(crashpad_client_test
  PRIVATE
  annotation_list_test.cc
  annotation_test.cc
  crash_report_database_test.cc
  prune_crash_reports_test.cc
  settings_test.cc
  simple_address_range_bag_test.cc
  simple_string_dictionary_test.cc
)

if(APPLE)
  target_sources(crashpad_client_test PRIVATE simulate_crash_mac_test.cc)
endif()

target_link_libraries(crashpad_client_test
  PRIVATE
  util
  snapshot
  crashpad_client
  crashpad_test
  crashpad_handler
  gtest
  gtest_main
  gmock
  mini_chromium_base
  ZlibInterface
)