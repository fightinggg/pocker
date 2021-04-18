cmake_minimum_required(VERSION 3.11)
project(pocker)

# 虚拟的linux头文件
include_directories(/Users/s/src/linux-include/)

set(SRC
        src/param/parse/ParamParse.cpp
        src/param/parse/RunParamParse.cpp
        src/main.cpp
        )


set(CMAKE_CXX_STANDARD 14)

add_executable(pocker ${SRC})
