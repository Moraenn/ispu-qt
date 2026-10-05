# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "AlarmWindow_autogen"
  "CMakeFiles/AlarmWindow_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/AlarmWindow_autogen.dir/ParseCache.txt"
  )
endif()
