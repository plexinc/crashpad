cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "")

project(crashpad C CXX)

if(NOT MSVC)
  enable_language(ASM)
else()
  enable_language(ASM_MASM)
endif()

enable_testing()

find_package(Threads)

add_library(ZlibInterface INTERFACE)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup(TARGETS NO_OUTPUT_DIRS KEEP_RPATHS)

  target_link_libraries(ZlibInterface INTERFACE CONAN_PKG::zlib)
else()
  find_package(ZLIB)
  target_link_libraries(ZlibInterface INTERFACE ZLIB::ZLIB)
endif()

if(APPLE)
  find_package(Python2 COMPONENTS Interpreter REQUIRED)
endif()

include(apple.cmake)

add_library(crashpad_common INTERFACE)

target_link_libraries(crashpad_common
  INTERFACE
  Threads::Threads
  ${CMAKE_DL_LIBS}
)

if(NOT MSVC)
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
    #-Wexit-time-destructors
    -Wimplicit-fallthrough

    -fno-exceptions
    -fno-rtti
    -fno-strict-aliasing
    -fobjc-call-cxx-cdtors
    -fstack-protector-all
    -fdata-sections
    -ffunction-sections
  )
else()
  include(windows.cmake)
endif()

if(APPLE)
  target_link_options(crashpad_common INTERFACE -Wl,-dead_strip)
endif()

if(ANDROID)
  target_link_libraries(crashpad_common INTERFACE -llog)
endif()

target_compile_definitions(crashpad_common
  INTERFACE
  -D_FILE_OFFSET_BITS=64
  -DCRASHPAD_ZLIB_SOURCE_SYSTEM
  -DCRASHPAD_LSS_SOURCE_EMBEDDED
)

target_include_directories(crashpad_common
  INTERFACE
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/third_party/mini_chromium/mini_chromium
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

  if(NOT ${TARGET} STREQUAL "compat")
    target_link_libraries(${TARGET} PRIVATE compat)
  endif()
endmacro()

include(GoogleTest)

add_custom_target(build_tests)

macro(crashpad_add_test NAME)
  add_executable(${NAME})
  crashpad_set_props(${NAME})
  if(APPLE)
    target_link_libraries(${NAME} PRIVATE AppleFrameworks)
  endif()
  if(NOT ANDROID)
    gtest_discover_tests(${NAME} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
  endif()
  add_dependencies(build_tests ${NAME})
endmacro()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(compat)

add_subdirectory(snapshot)
add_subdirectory(util)
add_subdirectory(third_party)
add_subdirectory(minidump)
add_subdirectory(client)
add_subdirectory(tools)
add_subdirectory(handler)

add_subdirectory(test)
