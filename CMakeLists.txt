cmake_minimum_required(VERSION 3.11)
project(pocker)

set(SRC
        src/param/parse/param_parse.cpp
        src/param/parse/run_param_parse.cpp
        src/main.cpp
        )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

set(CMAKE_CXX_STANDARD 14)

add_executable(pocker ${SRC})
