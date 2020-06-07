cmake_minimum_required(VERSION 3.16.0)
include_directories(${PROJECT_SOURCE_DIR}/include/third_party)
include_directories(${PROJECT_SOURCE_DIR}/include/base)
# set(CMAKE_SYSTEM_VERSION "8.1" CACHE STRING INTERNAL FORCE)
# set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION "8.1" CACHE STRING INTERNAL FORCE)

aux_source_directory(./messageloop/ DIR_SRCS)
aux_source_directory(./closure/ DIR_SRCS)
aux_source_directory(./messagepump/ DIR_SRCS)
aux_source_directory(./synchronization/ DIR_SRCS)
aux_source_directory(./time/ DIR_SRCS)
aux_source_directory(./thread/ DIR_SRCS)
aux_source_directory(. DIR_SRCS)
add_definitions(-DOS_WIN)

add_library(base STATIC ${DIR_SRCS})
set_property(TARGET base PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
# target_precompile_headers(base PRIVATE pch.h)