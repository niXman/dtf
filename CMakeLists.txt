
cmake_minimum_required(VERSION 2.8)
project(dtf)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")

include_directories(
	./
)

set(SOURCES
	./dtf.hpp
	./dtf.cpp
	./main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
