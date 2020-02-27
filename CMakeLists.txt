cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(crashpad C CXX ASM)

find_package(Threads)
find_package(ZLIB)

add_library(crashpad_common INTERFACE)

target_link_libraries(crashpad_common
  INTERFACE
  Threads::Threads
  ${CMAKE_DL_LIBS}
)

target_compile_options(crashpad_common
  INTERFACE
  -Wall
  -Wendif-labels
  -Werror
  -Wextra
  -Wextra-semi
  -Wno-missing-field-initializers
  -Wno-unused-parameter
  -Wsign-compare
  -Wheader-hygiene
  -Wnewline-eof
  -Wstring-conversion
  -Wexit-time-destructors
  -Wimplicit-fallthrough

  -fno-exceptions
  -fno-rtti
  -fno-strict-aliasing
  -fobjc-call-cxx-cdtors
  -fstack-protector-all
  -fdata-sections
  -ffunction-sections
)

target_compile_definitions(crashpad_common
  INTERFACE
  -D_FILE_OFFSET_BITS=64
  -DCRASHPAD_ZLIB_SOURCE_SYSTEM
  -DCRASHPAD_LSS_SOURCE_EMBEDDED
)

target_include_directories(crashpad_common
  INTERFACE
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/compat/linux
  ${CMAKE_SOURCE_DIR}/compat/non_mac
  ${CMAKE_SOURCE_DIR}/compat/non_win
  ${CMAKE_SOURCE_DIR}/../mini_chromium
  ${CMAKE_SOURCE_DIR}/../lss
)

macro(crashpad_set_props TARGET)
  set_target_properties(${TARGET}
    PROPERTIES
    CXX_STANDARD 14
    POSITION_INDEPENDENT_CODE ON
    CXX_VISIBILITY_PRESET "hidden"
    C_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN ON
  )

  target_link_libraries(${TARGET} PRIVATE crashpad_common)
endmacro()

add_subdirectory(compat)

target_link_libraries(crashpad_common INTERFACE compat)

add_subdirectory(snapshot)
add_subdirectory(util)
add_subdirectory(third_party)
add_subdirectory(minidump)
add_subdirectory(client)
add_subdirectory(tools)
add_subdirectory(handler)