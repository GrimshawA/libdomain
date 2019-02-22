cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_CXX_STANDARD 14)
project(libdom)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES} src/dom.hpp)
