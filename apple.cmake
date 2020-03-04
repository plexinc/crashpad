add_library(AppleFrameworks INTERFACE)

if(NOT APPLE)
  return()
endif()

set(FRAMEWORKS CoreFoundation;ApplicationServices;Foundation;IOKit;Security;bsm;OpenCL)
foreach(FW ${FRAMEWORKS})
  find_library(FW_PATH_${FW} ${FW})
  target_link_libraries(AppleFrameworks INTERFACE ${FW_PATH_${FW}})
endforeach()
