
cmake_minimum_required(VERSION 3.5)
project(dtf CXX)

option(ENABLE_ASAN "Enable AddressSanitizer")
option(ENABLE_UBSAN "Enable UndefinedBehaviourSanitizer")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(ENABLE_ASAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
endif()

if(ENABLE_UBSAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
endif()

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

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
