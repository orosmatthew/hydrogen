cmake_minimum_required(VERSION 3.20)

project(hydrogen)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES
        src/main.cpp)

add_executable(hydro)

target_sources(hydro PRIVATE ${SOURCE_FILES})

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_link_libraries(hydro -static-libgcc -static-libstdc++)
endif ()

