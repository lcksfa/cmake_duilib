#Cmake file for XmlPreview
#Author: lcksfa(yousaid820@163.com)
#Date: 2018/11/30

aux_source_directory(. DIR_SRCS)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_executable(XmlPreview ${DIR_SRCS})
set_target_properties(XmlPreview PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
add_dependencies(XmlPreview duilib)
target_link_libraries(XmlPreview duilib)
add_custom_command(TARGET XmlPreview POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  ${PROJECT_BINARY_DIR}/bin/XmlPreview.exe ${PROJECT_SOURCE_DIR}/bin/XmlPreview.exe)
