cmake_minimum_required(VERSION 3.16.0)

set(LANGUAGES CXX C)
include_directories(${PROJECT_SOURCE_DIR}/include/base)
include_directories(${PROJECT_SOURCE_DIR}/include/utility)
include_directories(${PROJECT_SOURCE_DIR}/include/dui)
include_directories(${PROJECT_SOURCE_DIR}/include/ui_components)
# set(CMAKE_SYSTEM_VERSION "8.1" CACHE STRING INTERNAL FORCE)
# set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION "8.1" CACHE STRING INTERNAL FORCE)

add_definitions(-DUILIB_STATIC)
aux_source_directory(./async_modal/ DIR_SRCS)
aux_source_directory(. DIR_SRCS)

add_library(ui_components STATIC ${DIR_SRCS})
target_link_libraries(ui_components utility duilib)
set_property(TARGET ui_components PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
# target_precompile_headers(base PRIVATE pch.h)