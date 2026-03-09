cmake_minimum_required(VERSION 3.10)

include_directories(${HEAD_PATH})
file(GLOB_RECURSE LOG ./*.cc)

set(LIBRARY_OUTPUT_PATH ${LIB_PATH})
add_library(${LOG_LIB_NAME} SHARED ${LOG})