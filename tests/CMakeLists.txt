cmake_minimum_required(VERSION 3.10)
project(qzxsqualpel_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(../)

file(GLOB TGT_SRC
    *.cpp
    *.h
        "../core/*.cpp"
        "../core/*.h"
        "../files/*.cpp"
        "../files/*.h"
        "../files/project/*.cpp"
        "../files/project/*.h"
        "../memory/*.cpp"
        "../memory/*.h"
        "../postprocessors/*.cpp"
        "../postprocessors/*.h"
        "../utils/*.cpp"
        "../utils/*.h"
)

add_executable(qzxsqualpel_tests ${TGT_SRC})

target_link_libraries(qzxsqualpel_tests)