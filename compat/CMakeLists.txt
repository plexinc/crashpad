add_library(compat INTERFACE)
#crashpad_set_props(compat)
#target_link_libraries(compat PRIVATE crashpad_common)

if(UNIX AND (NOT APPLE))
  target_include_directories(compat INTERFACE linux)
  target_sources(compat
    PRIVATE
    linux/sys/mman.cc
  )
else()
  target_include_directories(compat INTERFACE non_elf)
endif()

if(NOT WIN)
  target_include_directories(compat INTERFACE non_win)
endif()

if(WIN)
  target_include_directories(compat INTERFACE win)
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
  )
endif()

if(APPLE)
  target_include_directories(compat INTERFACE mac)
else()
  target_include_directories(compat INTERFACE non_mac)
endif()
