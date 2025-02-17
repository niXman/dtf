
cmake_minimum_required(VERSION 2.8)
project(dtf)

option(ENABLE_ASAN "Enable AddressSanitizer")
option(ENABLE_UBSAN "Enable UndefinedBehaviourSanitizer")

if(ENABLE_ASAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
endif()

if(ENABLE_UBSAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
endif()

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")

message("CMDLINE: ${CMAKE_CXX_FLAGS}")

add_definitions(
    -UNDEBUG
)

include_directories(
    ../include
)

set(SOURCES
    ../include/dtf/dtf.hpp
    ./main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
