cmake_minimum_required(VERSION 4.2)

project(MyGameProject 
    VERSION 1.0 
    LANGUAGES CXX)

find_package(googletest QUIET)

add_executable(${PROJECT_NAME})
file(GLOB_RECURSE SOURCES src/*.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE lz4_static)