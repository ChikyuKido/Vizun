cmake_minimum_required(VERSION 3.28)
project(c-template)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

include_directories(src)

set(SOURCE_FILES
        src/main.cpp
)
add_executable(c-template ${SOURCE_FILES})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Configuring for Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
    add_definitions(-DDEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Configuring for Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
    add_definitions(-DDEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "Dist-Release")
    message("Configuring for Dist-Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -fomit-frame-pointer -flto -DNDEBUG -ffast-math")
endif ()
