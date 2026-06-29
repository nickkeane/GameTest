cmake_minimum_required(VERSION 4.2 FATAL_ERROR)
project(
    glad
    VERSION 1.0.0
    LANGUAGES C CXX
)

# Define glad::glad as a static library (*.lib) to be compiled into the game
add_library(${PROJECT_NAME} STATIC src/gl.c src/vulkan.c)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PUBLIC c_std_23)
set_target_properties(${PROJECT_NAME} PROPERTIES
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS OFF
)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)