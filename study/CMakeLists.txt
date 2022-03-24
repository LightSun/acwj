cmake_minimum_required(VERSION 3.5)

project(acwj LANGUAGES C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB CUR_H  *.h)
file(GLOB CUR_SRC  *.c)
file(GLOB CUR_H_CONTENT  content/*.h)
file(GLOB CUR_SRC_CONTENT  content/*.c)
file(GLOB CUR_H_WRITER  writer/*.h)
file(GLOB CUR_SRC_WRITER  writer/*.c)
file(GLOB CUR_H_REGISTER  register/*.h)
file(GLOB CUR_SRC_REGISTER  register/*.c)

source_group("Include" FILES
    ${CUR_H}
    ${CUR_H_CONTENT}
    ${CUR_H_WRITER}
    ${CUR_H_REGISTER}
    )
source_group("Source" FILES
    ${CUR_SRC}
    ${CUR_SRC_CONTENT}
    ${CUR_SRC_WRITER}
    ${CUR_SRC_REGISTER}
    )
add_executable(acwj
    ${CUR_H}
    ${CUR_H_CONTENT}
    ${CUR_H_WRITER}
    ${CUR_H_REGISTER}
    ${CUR_SRC}
    ${CUR_SRC_CONTENT}
    ${CUR_SRC_WRITER}
    ${CUR_SRC_REGISTER}
    )
