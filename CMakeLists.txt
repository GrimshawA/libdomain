cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_CXX_STANDARD 14)
project(libdomain LANGUAGES C CXX)
enable_language(CXX)

include_directories(src/)

set(QT_SAMPLES true)

if (QT_SAMPLES)
    # app sample
    add_subdirectory(app)
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h*")

add_library(libdom SHARED ${SOURCES} src/dom.hpp src/dom_schema.hpp)
set_target_properties(libdom PROPERTIES LINKER_LANGUAGE CXX)

# Terminal sample
add_executable(term main.cpp)
target_link_libraries(term libdom)
