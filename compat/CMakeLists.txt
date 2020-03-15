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

if(ANDROID)
  target_include_directories(compat PUBLIC android)
  target_sources(compat
    PRIVATE
    android/android/api-level.cc
    android/android/api-level.h
    android/dlfcn_internal.cc
    android/dlfcn_internal.h
    android/elf.h
    android/linux/elf.h
    android/linux/prctl.h
    android/linux/ptrace.h
    android/sched.h
    android/sys/epoll.cc
    android/sys/epoll.h
    android/sys/mman.cc
    android/sys/mman.h
    android/sys/syscall.h
    android/sys/user.h
  )
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
