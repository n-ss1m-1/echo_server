cmake_minimum_required(VERSION 3.10)

include_directories(${HEAD_PATH})
file(GLOB_RECURSE SRC ./*.cc)
list(REMOVE_ITEM SRC ./main.cc)

set(LIBRARY_OUTPUT_PATH ${LIB_PATH})
add_library(${SRC_LIB_NAME} SHARED ${SRC})


set(EXECUTABLE_OUTPUT_PATH ${EXEC_PAHT})
add_executable(${EXEC_NAME} ./main.cc)

target_compile_options(main PRIVATE -Wall -Wextra -g -O0)

link_directories(${LIB_PATH})
target_link_libraries(${EXEC_NAME} pthread ${SRC_LIB_NAME} ${LOG_LIB_NAME})
