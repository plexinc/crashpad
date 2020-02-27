add_library(tool_support OBJECT)
crashpad_set_props(tool_support)

target_sources(tool_support PRIVATE tool_support.cc tool_support.h)