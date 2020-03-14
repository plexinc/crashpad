if(APPLE)
  add_library(compat INTERFACE)
else()
  add_library(compat OBJECT)
  crashpad_set_props(compat)
endif()

if(UNIX AND (NOT APPLE))
  target_include_directories(compat PUBLIC linux)
  target_sources(compat
    PRIVATE
    linux/sys/mman.cc
  )
else()
  target_include_directories(compat INTERFACE non_elf)
endif()

if(NOT WIN32)
  target_include_directories(compat PUBLIC non_win)
endif()

if(WIN32)
  target_include_directories(compat PUBLIC win)
  target_sources(compat
    PRIVATE
    win/getopt.h
    win/strings.cc
    win/strings.h
    win/sys/types.h
    win/time.cc
    win/time.h
    win/winbase.h
    win/winnt.h
    win/winternl.h
    ${CMAKE_SOURCE_DIR}/third_party/getopt/getopt.cc
  )
endif()

if(APPLE)
  target_include_directories(compat INTERFACE mac)
else()
  target_include_directories(compat PUBLIC non_mac)
endif()
