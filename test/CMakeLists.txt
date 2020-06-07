cmake_minimum_required(VERSION 3.16.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(${PROJECT_SOURCE_DIR}/include/base)
include_directories(${PROJECT_SOURCE_DIR}/include/utility)
# set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)
add_library(catch2_main STATIC catch_main.cpp)
target_include_directories(catch2_main PUBLIC Catch2/single_include/catch2/)
set_property(TARGET catch2_main PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")


#设置3方库的路径
link_directories(${PROJECT_SOURCE_DIR}/3rdlib)

# 暂时不指定，因为这样会让VScode的单元测试插件显示异常
# 指定生成路径
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

aux_source_directory(./testcase/ DIR_SRCS)
add_executable(unittest ${DIR_SRCS})
target_include_directories(unittest PUBLIC Catch2/single_include/catch2/)
target_link_libraries(unittest catch2_main base utility fmt )
set_property(TARGET unittest PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
# pch
target_compile_definitions(unittest PRIVATE CATCH_CONFIG_FAST_COMPILE CATCH_CONFIG_DISABLE_MATCHERS)
target_precompile_headers(unittest PRIVATE Catch2/single_include/catch2/catch.hpp)


add_test(
  NAME unittest
  COMMAND $<TARGET_FILE:unittest> --success
  )