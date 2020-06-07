cmake_minimum_required(VERSION 3.16.0)
include_directories(${PROJECT_SOURCE_DIR}/include/third_party)
include_directories(${PROJECT_SOURCE_DIR}/include/base)
include_directories(${PROJECT_SOURCE_DIR}/include/utility)
include_directories(${PROJECT_SOURCE_DIR}/include/dui)
include_directories(${PROJECT_SOURCE_DIR}/include/ui_components)
add_definitions(-DUNICODE -D_UNICODE -DUILIB_STATIC)

aux_source_directory(./module/ DIR_SRCS)
aux_source_directory(. DIR_SRCS)

# 指定生成路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# IF(WIN32)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows") 
# ENDIF(WIN32)

#设置3方库的路径
link_directories(${PROJECT_SOURCE_DIR}/3rdlib)

#设置管理员权限
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /level='requireAdministrator' /uiAccess='false'")

#use the lib for unittest 
add_library(EAccopluslib ${DIR_SRCS} )
add_executable(EAccoplus ${DIR_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/EAccoplus.rc)
set_property(TARGET EAccoplus PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
             
set_property(TARGET EAccopluslib PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")         
target_link_libraries(EAccoplus base duilib  utility  fmt libcrypto32MD libcurl libssl32MD ui_components sqlite3 libxl)
target_link_libraries(EAccopluslib base duilib  utility  fmt libcrypto32MD libcurl libssl32MD ui_components sqlite3 libxl)