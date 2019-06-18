cmake_minimum_required(VERSION 2.8.12)

project(app LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Quick REQUIRED)

file(GLOB_RECURSE SOURCES "src/*")

add_executable(${PROJECT_NAME} ${SOURCES} "main.cpp" "qml.qrc"
    #"assets/assets.qrc"
    )

#target_link_libraries(app libdom)
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Quick)

include_directories("../src" "src")
