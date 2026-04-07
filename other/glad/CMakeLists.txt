cmake_minimum_required(VERSION 4.2)
project(
    glad
    VERSION 1.0.0
    LANGUAGES C CXX
)

# Define glad::glad as a static library (*.lib) to be compiled into our game
add_library(${PROJECT_NAME} STATIC src/gl.c src/vulkan.c)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
#target_include_directories(${PROJECT_NAME} PUBLIC
#    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#    $<INSTALL_INTERFACE:include>
#)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_compile_features(${PROJECT_NAME} PUBLIC c_std_23)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_26)

if(WIN32)
    # Force OGL + Vulkan calls to use __stdcall on Windows. __stdcall is ignored on x64 harmlessly.
    target_compile_definitions(${PROJECT_NAME} PUBLIC APIENTRY=__stdcall)
endif()
if(APPLE)
    # Apple has an additional MoltenVK library
    find_package(Vulkan REQUIRED COMPONENTS MoltenVK)
else()
    # Get Vulkan::Vulkan and Vulkan::Headers
    find_package(Vulkan REQUIRED)
endif()

# Ensure Vulkan is included IF using Vulkan Loader version of GLAD
# TODO: Decide if we will use LOADER version of Vulkan and/or OpenGL
# Vulkan will have to be included with app project if using Loader
#target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)

# The below should be done in the app code
## OpenGL SDK
#find_package(OpenGL REQUIRED)
#target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

## Vulkan SDK
#find_package(Vulkan REQUIRED)
#target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan Vulkan::Headers)
# if APPLE
#target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan Vulkan::Headers Vulkan::MoltenVK)