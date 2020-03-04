add_library(snapshot OBJECT)
crashpad_set_props(snapshot)
target_link_libraries(snapshot PRIVATE mini_chromium_base)

target_sources(snapshot
  PRIVATE
  annotation_snapshot.cc
  annotation_snapshot.h
  capture_memory.cc
  capture_memory.h
  cpu_architecture.h
  cpu_context.cc
  cpu_context.h
  crashpad_info_client_options.cc
  crashpad_info_client_options.h
  exception_snapshot.h
  handle_snapshot.cc
  handle_snapshot.h
  memory_snapshot.cc
  memory_snapshot.h
  memory_snapshot_generic.h
  minidump/exception_snapshot_minidump.cc
  minidump/exception_snapshot_minidump.h
  minidump/memory_snapshot_minidump.cc
  minidump/memory_snapshot_minidump.h
  minidump/minidump_annotation_reader.cc
  minidump/minidump_annotation_reader.h
  minidump/minidump_context_converter.cc
  minidump/minidump_context_converter.h
  minidump/minidump_simple_string_dictionary_reader.cc
  minidump/minidump_simple_string_dictionary_reader.h
  minidump/minidump_stream.h
  minidump/minidump_string_list_reader.cc
  minidump/minidump_string_list_reader.h
  minidump/minidump_string_reader.cc
  minidump/minidump_string_reader.h
  minidump/module_snapshot_minidump.cc
  minidump/module_snapshot_minidump.h
  minidump/process_snapshot_minidump.cc
  minidump/process_snapshot_minidump.h
  minidump/system_snapshot_minidump.cc
  minidump/system_snapshot_minidump.h
  minidump/thread_snapshot_minidump.cc
  minidump/thread_snapshot_minidump.h
  module_snapshot.h
  process_snapshot.h
  snapshot_constants.h
  system_snapshot.h
  thread_snapshot.h
  unloaded_module_snapshot.cc
  unloaded_module_snapshot.h
)

if(UNIX)
  target_sources(snapshot
    PRIVATE
    posix/timezone.cc
    posix/timezone.h
  )
  if(NOT APPLE)
    target_sources(snapshot
      PRIVATE
      linux/cpu_context_linux.cc
      linux/cpu_context_linux.h
      linux/debug_rendezvous.cc
      linux/debug_rendezvous.h
      linux/exception_snapshot_linux.cc
      linux/exception_snapshot_linux.h
      linux/process_reader_linux.cc
      linux/process_reader_linux.h
      linux/process_snapshot_linux.cc
      linux/process_snapshot_linux.h
      linux/signal_context.h
      linux/system_snapshot_linux.cc
      linux/system_snapshot_linux.h
      linux/thread_snapshot_linux.cc
      linux/thread_snapshot_linux.h
      sanitized/memory_snapshot_sanitized.cc
      sanitized/memory_snapshot_sanitized.h
      sanitized/module_snapshot_sanitized.cc
      sanitized/module_snapshot_sanitized.h
      sanitized/process_snapshot_sanitized.cc
      sanitized/process_snapshot_sanitized.h
      sanitized/sanitization_information.cc
      sanitized/sanitization_information.h
      sanitized/thread_snapshot_sanitized.cc
      sanitized/thread_snapshot_sanitized.h
      crashpad_types/image_annotation_reader.cc
      crashpad_types/image_annotation_reader.h
      elf/elf_dynamic_array_reader.cc
      elf/elf_dynamic_array_reader.h
      elf/elf_image_reader.cc
      elf/elf_image_reader.h
      elf/elf_symbol_table_reader.cc
      elf/elf_symbol_table_reader.h
      elf/module_snapshot_elf.cc
      elf/module_snapshot_elf.h
    )
  endif()
endif()

if(APPLE)
  target_sources(snapshot
    PRIVATE
    mac/cpu_context_mac.cc
    mac/cpu_context_mac.h
    mac/exception_snapshot_mac.cc
    mac/exception_snapshot_mac.h
    mac/mach_o_image_annotations_reader.cc
    mac/mach_o_image_annotations_reader.h
    mac/mach_o_image_reader.cc
    mac/mach_o_image_reader.h
    mac/mach_o_image_segment_reader.cc
    mac/mach_o_image_segment_reader.h
    mac/mach_o_image_symbol_table_reader.cc
    mac/mach_o_image_symbol_table_reader.h
    mac/module_snapshot_mac.cc
    mac/module_snapshot_mac.h
    mac/process_reader_mac.cc
    mac/process_reader_mac.h
    mac/process_snapshot_mac.cc
    mac/process_snapshot_mac.h
    mac/process_types.cc
    mac/process_types.h
    mac/process_types/custom.cc
    mac/process_types/flavors.h
    mac/process_types/internal.h
    mac/process_types/traits.h
    mac/system_snapshot_mac.cc
    mac/system_snapshot_mac.h
    mac/thread_snapshot_mac.cc
    mac/thread_snapshot_mac.h
  )
endif()

if(UNIX OR WIN AND NOT APPLE)
  target_sources(snapshot
    PRIVATE
    crashpad_types/crashpad_info_reader.cc
    crashpad_types/crashpad_info_reader.h
  )
endif()

if(WIN)
  target_sources(snapshot
    PRIVATE
    win/capture_memory_delegate_win.cc
    win/capture_memory_delegate_win.h
    win/cpu_context_win.cc
    win/cpu_context_win.h
    win/exception_snapshot_win.cc
    win/exception_snapshot_win.h
    win/memory_map_region_snapshot_win.cc
    win/memory_map_region_snapshot_win.h
    win/module_snapshot_win.cc
    win/module_snapshot_win.h
    win/pe_image_annotations_reader.cc
    win/pe_image_annotations_reader.h
    win/pe_image_reader.cc
    win/pe_image_reader.h
    win/pe_image_resource_reader.cc
    win/pe_image_resource_reader.h
    win/process_reader_win.cc
    win/process_reader_win.h
    win/process_snapshot_win.cc
    win/process_snapshot_win.h
    win/process_subrange_reader.cc
    win/process_subrange_reader.h
    win/system_snapshot_win.cc
    win/system_snapshot_win.h
    win/thread_snapshot_win.cc
    win/thread_snapshot_win.h
  )
endif()

message(STATUS ${CMAKE_SYSTEM_PROCESSOR})

if(CMAKE_SYSTEM_PROCESSOR STREQUAL x86_64)
  target_sources(snapshot
    PRIVATE
    x86/cpuid_reader.cc
    x86/cpuid_reader.h
  )
endif()


add_library(snapshot_test_support OBJECT)
crashpad_set_props(snapshot_test_support)
target_sources(snapshot_test_support
  PRIVATE
  test/test_cpu_context.cc
  test/test_cpu_context.h
  test/test_exception_snapshot.cc
  test/test_exception_snapshot.h
  test/test_memory_map_region_snapshot.cc
  test/test_memory_map_region_snapshot.h
  test/test_memory_snapshot.cc
  test/test_memory_snapshot.h
  test/test_module_snapshot.cc
  test/test_module_snapshot.h
  test/test_process_snapshot.cc
  test/test_process_snapshot.h
  test/test_system_snapshot.cc
  test/test_system_snapshot.h
  test/test_thread_snapshot.cc
  test/test_thread_snapshot.h
)

target_link_libraries(snapshot_test_support PRIVATE mini_chromium_base util)

crashpad_add_test(crashpad_snapshot_test)
target_sources(crashpad_snapshot_test
  PRIVATE
  cpu_context_test.cc
  memory_snapshot_test.cc
  minidump/process_snapshot_minidump_test.cc
)

if(APPLE)
  target_sources(crashpad_snapshot_test
    PRIVATE
    mac/cpu_context_mac_test.cc
    mac/mach_o_image_annotations_reader_test.cc
    mac/mach_o_image_reader_test.cc
    mac/mach_o_image_segment_reader_test.cc
    mac/process_reader_mac_test.cc
    mac/process_types_test.cc
    mac/system_snapshot_mac_test.cc
  )
endif()

if(UNIX AND NOT APPLE)
  target_sources(crashpad_snapshot_test
    PRIVATE
    linux/debug_rendezvous_test.cc
    linux/exception_snapshot_linux_test.cc
    linux/process_reader_linux_test.cc
    linux/system_snapshot_linux_test.cc
    sanitized/process_snapshot_sanitized_test.cc
    sanitized/sanitization_information_test.cc
  )
else()
  target_sources(crashpad_snapshot_test PRIVATE crashpad_info_client_options_test.cc)
endif()

if(NOT APPLE)
  target_sources(crashpad_snapshot_test
    PRIVATE
    crashpad_types/crashpad_info_reader_test.cc
  )
endif()

if(WIN)
  target_sources(crashpad_snapshot_test
    PRIVATE
    win/cpu_context_win_test.cc
    win/exception_snapshot_win_test.cc
    win/extra_memory_ranges_test.cc
    win/module_snapshot_win_test.cc
    win/pe_image_reader_test.cc
    win/process_reader_win_test.cc
    win/process_snapshot_win_test.cc
    win/system_snapshot_win_test.cc
  )
else()
  target_sources(crashpad_snapshot_test PRIVATE posix/timezone_test.cc)
endif()

target_link_libraries(crashpad_snapshot_test
  PRIVATE
  snapshot_test_support
  crashpad_client
  crashpad_test
  mini_chromium_base
  util
  gtest
  gtest_main
  crashpad_handler
)

add_dependencies(crashpad_snapshot_test
  crashpad_snapshot_test_module
  crashpad_snapshot_test_module_large
  crashpad_snapshot_test_module_small
)

set(CMAKE_SHARED_MODULE_PREFIX "")

add_library(crashpad_snapshot_test_module MODULE crashpad_info_client_options_test_module.cc)
crashpad_set_props(crashpad_snapshot_test_module)
target_link_libraries(crashpad_snapshot_test_module PRIVATE crashpad_client AppleFrameworks)

add_library(crashpad_snapshot_test_module_large MODULE crashpad_info_size_test_module.cc)
crashpad_set_props(crashpad_snapshot_test_module_large)
target_compile_definitions(crashpad_snapshot_test_module_large PRIVATE CRASHPAD_INFO_SIZE_TEST_MODULE_LARGE)
target_link_libraries(crashpad_snapshot_test_module_large PRIVATE util AppleFrameworks)

add_library(crashpad_snapshot_test_module_small MODULE crashpad_info_size_test_module.cc)
crashpad_set_props(crashpad_snapshot_test_module_small)
target_compile_definitions(crashpad_snapshot_test_module_small PRIVATE CRASHPAD_INFO_SIZE_TEST_MODULE_SMALL)
target_link_libraries(crashpad_snapshot_test_module_large PRIVATE util AppleFrameworks)

if(UNIX AND NOT APPLE)
  add_library(crashpad_snapshot_test_both_dt_hash_styles MODULE hash_types_test.cc)
  target_link_options(crashpad_snapshot_test_both_dt_hash_styles PRIVATE "-Wl,--hash-style=both")
endif()

if(APPLE)
  add_library(crashpad_snapshot_test_module_crashy_initializer
    MODULE
    mac/mach_o_image_annotations_reader_test_module_crashy_initializer.cc
  )
  crashpad_set_props(crashpad_snapshot_test_module_crashy_initializer)

  add_executable(crashpad_snapshot_test_no_op mac/mach_o_image_annotations_reader_test_no_op.cc)
  crashpad_set_props(crashpad_snapshot_test_no_op)

  add_dependencies(crashpad_snapshot_test
    crashpad_snapshot_test_module_crashy_initializer
    crashpad_snapshot_test_no_op
  )
endif()