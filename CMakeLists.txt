cmake_minimum_required(VERSION 3.10)
project(muduo)

set(CMAKE_CXX_STANDARD 11)

#设置全局变量
set(LIB_PATH ${PROJECT_SOURCE_DIR}/lib)
set(EXEC_PATH ${PROJECT_SOURCE_DIR}/bin)
set(HEAD_PATH ${PROJECT_SOURCE_DIR}/include)

#设置名称
set(SRC_LIB_NAME src)
set(LOG_LIB_NAME log)
set(EXEC_NAME main)

#添加子目录
add_subdirectory(./src)
add_subdirectory(./log)

