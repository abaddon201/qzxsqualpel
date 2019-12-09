cmake_minimum_required(VERSION 3.10)
project(qzxsqualpel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "c:\\Qt\\5.12.6\\msvc2017_64\\lib\\cmake")

include_directories(utils)

file(GLOB TGT_SRC  main.cpp
        "core/*.cpp"
        "core/*.h"
        "files/*.cpp"
        "files/*.h"
        "files/project/*.cpp"
        "files/project/*.h"
        "memory/*.cpp"
        "memory/*.h"
        "postprocessors/*.cpp"
        "postprocessors/*.h"
        "utils/*.cpp"
        "utils/*.h"
        "utils/plog/*.h"
        "gui/*.cpp"
        "gui/*.ui"
        "gui/*.h")

find_package(Qt5Widgets REQUIRED)

add_executable(qzxsqualpel ${TGT_SRC})

target_link_libraries(qzxsqualpel Qt5::Widgets)

add_subdirectory(tests)