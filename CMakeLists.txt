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
  find_package(ZLib)
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
  target_compile_definitions(crashpad_common
    INTERFACE
    NOMINMAX
    UNICODE
    WIN32_LEAN_AND_MEAN
    _CRT_SECURE_NO_WARNINGS
    _HAS_EXCEPTIONS=0
    _UNICODE
  )

  target_compile_options(crashpad_common
    INTERFACE
    $<$<COMPILE_LANGUAGE:CXX>:
      /FS
      /W4
      /WX
      /Zi
      /bigobj
      /wd4100
      /wd4127
      /wd4324
      /wd4351
      /wd4577
      /wd4996
      /wd4201
      /Zc:inline
      /d2Zi+
    >
  )

  target_link_libraries(crashpad_common
    INTERFACE
    advapi32.lib
    winhttp.lib
    version.lib
    user32.lib
    PowrProf.lib
  )

endif()

if(APPLE)
  target_link_options(crashpad_common INTERFACE -Wl,-dead_strip)
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
endmacro()

include(GoogleTest)

add_custom_target(build_tests)

macro(crashpad_add_test NAME)
  add_executable(${NAME})
  crashpad_set_props(${NAME})
  if(APPLE)
    target_link_libraries(${NAME} PRIVATE AppleFrameworks)
  endif()
  gtest_discover_tests(${NAME} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
  add_dependencies(build_tests ${NAME})
endmacro()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(compat)

target_link_libraries(crashpad_common INTERFACE compat)

add_subdirectory(snapshot)
add_subdirectory(util)
add_subdirectory(third_party)
add_subdirectory(minidump)
add_subdirectory(client)
add_subdirectory(tools)
add_subdirectory(handler)

add_subdirectory(test)
