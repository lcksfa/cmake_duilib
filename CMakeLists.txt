# CMake root file for cmake_duilib related apps
#Date 2018-11-30
#Author: Lcksfa(yousaid820@163.com)
#
#  CMake files for cmake_duilib
#  file for each app is under the related sub-dir
#

cmake_minimum_required(VERSION 3.0)

project(cmake_duilib)

message(STATUS," CMake project files for cmake_duilib")

# this line is for UNICODE release,which is required by DuiDesigner 
add_definitions(-DUNICODE -D_UNICODE)

# add each CMake file
add_subdirectory(duilib)
add_subdirectory(XmlPreview)
