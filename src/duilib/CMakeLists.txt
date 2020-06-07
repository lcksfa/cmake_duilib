cmake_minimum_required(VERSION 3.16.0)
include_directories(${PROJECT_SOURCE_DIR}/include/3rd)
# include_directories(${PROJECT_SOURCE_DIR}/include/base)
include_directories(${PROJECT_SOURCE_DIR}/include/dui)


add_definitions(-DUILIB_STATIC)
aux_source_directory(./control/ DIR_SRCS)
aux_source_directory(./core/ DIR_SRCS)
aux_source_directory(./layout/ DIR_SRCS)
aux_source_directory(./utils/ DIR_SRCS)
aux_source_directory(./ DIR_SRCS)

add_library(duilib STATIC ${DIR_SRCS})
set_property(TARGET duilib PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
target_link_libraries(duilib)
