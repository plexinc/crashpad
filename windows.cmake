if(NOT WIN32)
  return()
endif()

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
  $<$<COMPILE_LANGUAGE:ASM_MASM>:/safeseh>
)

target_link_libraries(crashpad_common
  INTERFACE
  advapi32.lib
  winhttp.lib
  version.lib
  user32.lib
  PowrProf.lib
)

target_link_options(crashpad_common
  INTERFACE
  /INCREMENTAL:NO
  /Debug:FASTLINK
)