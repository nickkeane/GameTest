cmake_minimum_required(VERSION 4.2)

project(MyGameProject 
    VERSION 1.0 
    DESCRIPTION "Game Test Project"
    LANGUAGES CXX)

include(FetchContent)

enable_testing()

# GTest Dependency
find_package(googletest 1.17.0 QUIET)
if (NOT googletest_FOUND)
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/other/googletest/CMakeLists.txt")
        message(STATUS "Building other/googletest...")
        add_subdirectory(other/googletest)
        set (googletest_FOUND true)
    else()
        message(STATUS "other/googletest not found. Downloading via git...")
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY      https://github.com/google/googletest.git
            GIT_TAG             v1.17.0
        )
        if (WIN32)
            set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        endif()
        FetchContent_MakeAvailable(googletest)
    endif()
else()
    message(STATUS "Found system googletest installation")
endif()


### GameEXE Library ###
########################
add_executable(${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
)
# target_compile_features(${PROJECT_NAME} PUBLIC c_std_11)
# set_target_properties(${PROJECT_NAME} PROPERTIES
#     C_EXTENSIONS OFF
#     C_STANDARD_REQUIRED ON
# )

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include)
#target_link_libraries(${PROJECT_NAME} PRIVATE lz4_static)
########################

add_subdirectory(test)