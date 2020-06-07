cmake_minimum_required(VERSION 3.16.0)

set(LANGUAGES CXX C)
include_directories(${PROJECT_SOURCE_DIR}/include/third_party)
include_directories(${PROJECT_SOURCE_DIR}/include/base)
include_directories(${PROJECT_SOURCE_DIR}/include/utility)
# set(CMAKE_SYSTEM_VERSION "8.1" CACHE STRING INTERNAL FORCE)
# set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION "8.1" CACHE STRING INTERNAL FORCE)

aux_source_directory(./app_env/ DIR_SRCS)
aux_source_directory(./win32/ DIR_SRCS)
aux_source_directory(./encrypt/ DIR_SRCS)
aux_source_directory(./http/ DIR_SRCS)
aux_source_directory(./db/ DIR_SRCS)
aux_source_directory(./json/ DIR_SRCS)
aux_source_directory(./toolset/ DIR_SRCS)
aux_source_directory(./log/ DIR_SRCS)
aux_source_directory(. DIR_SRCS)

add_library(utility STATIC ${DIR_SRCS})
target_link_libraries(utility sqlite3)
set_property(TARGET utility PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
# target_precompile_headers(base PRIVATE pch.h)