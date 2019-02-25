cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_CXX_STANDARD 14)
project(libdomain LANGUAGES C CXX)
enable_language(CXX)

include_directories(src/)

# app sample
add_subdirectory(app)


file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h*")

add_library(libdom SHARED ${SOURCES} src/dom.hpp)
set_target_properties(libdom PROPERTIES LINKER_LANGUAGE CXX)

# Terminal sample
add_executable(term main.cpp)
target_link_libraries(term libdom)
