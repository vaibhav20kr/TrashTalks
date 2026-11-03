# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  [[CMakeFiles\TrashTalk_autogen.dir\AutogenUsed.txt]]
  [[CMakeFiles\TrashTalk_autogen.dir\ParseCache.txt]]
  "TrashTalk_autogen"
  )
endif()
