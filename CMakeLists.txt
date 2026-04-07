cmake_minimum_required(VERSION 4.2)

#[[
    Project Definition
]]
project(MyGameProject
    VERSION 1.0.0
    DESCRIPTION "Game Test Project"
    HOMEPAGE_URL "https://github.com/nickkeane/GameTest"
    LANGUAGES C CXX ASM_NASM
)

#[[
    CMake Includes
]]
include(CheckSymbolExists)
include(CheckIPOSupported)
include(FetchContent)

#[[
    Set CMake Configuration Targets

    I like to restrict it to just Debug and Release
    The correct flags and definitions should be passed to each toolset for the builds
    CMAKE_CONFIGURATION_TYPES and CMAKE_BUILD_TYPE should be specified in the conan profiles
    for Visual Studio, as it defaults to Debug;Release;MinSizeRel;RelWithDebInfo.
]]
if(NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
endif()

#[[
    Check for IPO/LTO Support

    Interprocedural Optimization (IPO) or Link Time Optimization (LTO) can provide significant performance
    improvements for Release builds by enabling optimizations across translation units.
    However, not all compilers and platforms support IPO/LTO, so check for support before enabling it.
]]
check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT IPO_ERROR)
if (IPO_SUPPORTED)
    message(STATUS "IPO/LTO is supported and will be enabled for Release builds.")
else()
    message(STATUS "IPO/LTO is not supported: ${IPO_ERROR}")
endif()

#[[
    - Adds an Executable Target (*.exe)
    - For a Static or Dynamic Library Target, use add_library() instead of add_executable()
    - For a Header-Only Library Target, use add_library() with the INTERFACE keyword and no sources.
]]
add_executable(${PROJECT_NAME})

#[[
    - Set Latest ISO C and C++ Standards to 2023
    - Disable custom C/C++ StdLib extensions (i.e. gnuYY vs stdYY)
    - Require strict adherence to the ISO Standards from toolsets
]]
target_compile_features(${PROJECT_NAME} PUBLIC c_std_23)
set_target_properties(${PROJECT_NAME} PROPERTIES
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS OFF
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

#[[
    Add Compiler Options
    - Warning Flags
    - FP Precise Mode
]]
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Enable output of all warnings + extra + pedantic warnings
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wextra)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wpedantic)
    # Convert all warnings to errors. Use with caution, as some warnings may be benign or intentional.
    #target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
    # Explicit FP Fast Mode - Usually defaults to FP Precise in GNU GCC or Clang
    target_compile_options(${PROJECT_NAME} PRIVATE -ffast-math)
    #
    target_compile_options(${PROJECT_NAME} PRIVATE -fstack-protector-strong)

    # Debug Build Targets
    # Enable extra asserts for Debug builds
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:-fno-omit-frame-pointer>)
    # Checks container bounds and smart pointer dereferencing.
    target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:_GLIBCXX_ASSERTIONS>)

    # Release Build Targets
    # V3 - Target 2017 x86-64 arch baseline (SSE4.2, AVX2, F16C) (Haswell, Zen 1)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:-march=x86-64-v3>)
    # V4 - Target 2022 x86-64 arch baseline (AVX512, etc.) (Skylake, Zen 4)
    # As of 2026, not a great target for consumer hardware.
    # My 2 year old i9 laptop doesn't even have support for AVX512 lmao.
    # Don't enable this until 2035 at the earliest.
    #target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:-march=x86-64-v4>)
    # Strip Symbols for linker.
    target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:-s>)
    # No container bounds or smart pointer dereferencing checks.
    target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:_GLIBCXX_NO_ASSERTIONS>)
elseif(MSVC)
    # AVX2 - Target 2017 x86-64 arch baseline (SSE4.2, AVX2, F16C) (Haswell, Zen 1)
    target_compile_options(${PROJECT_NAME} PRIVATE /arch:AVX2)
    # AVX512 - Target 2022 x86-64 arch baseline (AVX512) (Skylake, Zen 4)
    # Don't enable this until 2035 at the earliest.
    #target_compile_options(${PROJECT_NAME} PRIVATE /arch:AVX512)
    # Warning Level 4 - Output all warnings
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
    target_compile_options(${PROJECT_NAME} PRIVATE /permissive-)
    # Enable UTF-8 Sources
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
    # Enable FP Fast Mode - Usually the default for MSVC
    target_compile_options(${PROJECT_NAME} PRIVATE /fp:fast)
    # Disable Microsoft's ISO C11 Annex K Implementation. Make sure to include safeclib headers
    # last after windows headers or C/C++ standard library headers to overwrite.
    target_compile_definitions(${PROJECT_NAME} PRIVATE __STDC_WANT_LIB_EXT1__=0)
    # Disable warnings about the use of "unsafe" CRT functions (i.e. printf vs printf_s)
    # in Visual Studio. *_s functions are not universally supported and have their own
    # issues, so its better to just use a library like safeclib for all platforms.
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _SCL_SECURE_NO_WARNINGS)
else()
    message(FATAL_ERROR "Unknown compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (IPO_SUPPORTED)
        # Link Time Optimization (Inlining, Dead Code Elimination, Devirtualization, CPU Register Realloc)
        # Can also be manually set with -flto to target_compile_options() and target_link_options() for GCC.
        set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(FATAL_ERROR "IPO/LTO is not supported for Release builds.")
    endif()
endif()

#[[
    Platform options and definitions
]]
if (WIN32)
    # Force Windows 10 SDK (0x0A00) Windows headers
    target_compile_definitions(${PROJECT_NAME} PRIVATE WINVER=0x0A00)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0A00)
    # Reduce little-used <windows.h> includes, such as GDI, Cryptography, DDE, and ShellAPI.
    target_compile_definitions(${PROJECT_NAME} PRIVATE WIN32_LEAN_AND_MEAN)
    # Use ISO C/C++ std::min/std::max instead of min and max macros, which can break some libraries.
    target_compile_definitions(${PROJECT_NAME} PRIVATE NOMINMAX)
    # Force strict Win32 API type checking (i.e. HWND vs HDC)
    # Modern compilers enable this by default, but it doesn't hurt to make explicit.
    target_compile_definitions(${PROJECT_NAME} PRIVATE STRICT)
    # Force Unicode (W) functions for Win32 API.
    # Can disable if wanting to use (A) functions and don't care about internationalization.
    # Best practice is to define UNICODE and use L"" string literals and std::wstring.c_str()
    # for any Win32 API calls involving strings. Oh, and use StdLib equivs wherever possible.
    # Old best practice is to use Win32 API function macros, the types TCHAR, LPTSTR, and LPCTSTR
    # to manage string objects and _T() or TEXT() to convert string literals, as well as <tchar.h>
    # variants of string operations on TCHAR types i.e. _tchlen() instead of strlen()
    # MultiByteToWideChar and WideCharToMultiByte can convert strings between UNICODE and ANSI
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
endif()

#[[
    External Libraries
    The Conan and CMake profiles provide the correct paths for external
    dependencies for specific build configs (UCRT64, VS2026 and Debug, Release)
]]

# ZLib Dependency
#find_package(ZLIB REQUIRED)

# GLAD (GL 4.6 + Vulkan 1.4 - Core)
# TODO: Add GLAD dependency, use https://gen.glad.sh/ to generate
# TODO: Integrate project with CMake
## add_subdirectory(other/glad)

##target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/other/glad/include)
##file(GLOB_RECURSE GLAD_SOURCES CONFIGURE_DEPENDS "other/glad/src/*.c")

##find_package(eastl REQUIRED)
##target_link_libraries(${PROJECT_NAME} PRIVATE eastl::eastl)

# Find Native Threads for std::thread support in C++11 and later
find_package(Threads REQUIRED)

# Packages
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
if (WIN32)
    # Windows / MSYS2 UCRT64 / Cygwin
    find_package(OpenGL REQUIRED)
    find_package(Vulkan REQUIRED)
    # SDL3 or GLFW should be used for windowing and input rather than raw Win32 API calls
    # The below are likely not needed unless you want raw Win32 API calls.
    #target_link_libraries(${PROJECT_NAME} PRIVATE user32)
    #target_link_libraries(${PROJECT_NAME} PRIVATE gdi32)
    # Winsock 2 for _WSAStartup and _WSACleanup pre-reqs for Boost.Asio and Boost.Beast
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Vulkan::Vulkan
            Vulkan::Headers
    )
elseif (UNIX AND NOT APPLE)
    # Linux
    # OpenGL + Linux X11 Windowing
    find_package(OpenGL REQUIRED)
    find_package(Vulkan REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE dl)
    # May already be handled by Threads::Threads. Enable if not.
    #target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
    target_link_libraries(${PROJECT_NAME} PRIVATE X11)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Vulkan::Vulkan
            Vulkan::Headers
    )
elseif(APPLE)
    # MacOS
    # OpenGL Core v4.x or Vulkan
    find_package(OpenGL REQUIRED)
    find_package(Vulkan REQUIRED COMPONENTS MoltenVK)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            "-framework CoreFoundation"
            "-framework CoreVideo"
            "-framework QuartzCore"
            "-framework IOKit"
            "-framework Cocoa"
            "-framework OpenGL"
            Vulkan::Vulkan
            Vulkan::Headers
            Vulkan::MoltenVK
    )
elseif(ANDROID)
    # Android
    # OpenGLES v3 or Vulkan
    find_package(Vulkan REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE android)
    target_link_libraries(${PROJECT_NAME} PRIVATE log)
    target_link_libraries(${PROJECT_NAME} PRIVATE GLESv3)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Vulkan::Vulkan
            Vulkan::Headers
    )
elseif(IOS)
    # iOS
    # OpenGLES is deprecated on iOS. Use Metal + MetalKit.
    target_link_libraries(${PROJECT_NAME} PRIVATE
            "-framework Foundation"
            "-framework UIKit"
            "-framework QuartzCore"
            "-framework CoreGraphics"
            "-framework CoreVideo"
            "-framework Metal"
            "-framework MetalKit"
    )
endif()

find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(simdjson REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(tomlplusplus REQUIRED)
find_package(Catch2 REQUIRED)
find_package(Boost REQUIRED)
find_package(Boost REQUIRED COMPONENTS
        asio # UDP/TCP/ICMP/Serial(COM)
        beast # WebSockets/HTTP1.1 (Includes Boost.asio)
        container # Containers, lots of overlap with StdLib
        coroutine2 # Stack-based coroutines, slower but more flexible than std::coroutine
        program_options # Argument Parsing
        regex # Regular Expressions
        serialization # Read/write objects to/from files
        signals2 # Signal/Slot Observer
        xpressive # Compile-Time RegEx
)
find_package(EnTT REQUIRED)
find_package(glm REQUIRED)

target_precompile_headers(${PROJECT_NAME} PRIVATE
        <fmt/core.h>
        <fmt/format.h>)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
target_precompile_headers(${PROJECT_NAME} PRIVATE
        <spdlog/spdlog.h>
        <spdlog/fmt/fmt.h>)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE simdjson::simdjson)
target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp::yaml-cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE tomlplusplus::tomlplusplus)
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2)
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${PROJECT_NAME} PRIVATE
        Boost::boost
#        Boost::asio
#        Boost::beast
        Boost::container
        Boost::context
#        Boost::coroutine2
        Boost::program_options
        Boost::regex
        Boost::serialization
#        Boost::signals2
#        Boost::xpressive
)
target_link_libraries(${PROJECT_NAME} PRIVATE EnTT::EnTT)
# Force GLM to use explicit constructors for type conversions (i.e. ivec4 a; vec4 b = a; would be an error. Use vec4 b(a) instead).
target_compile_definitions(${PROJECT_NAME} PRIVATE GLM_FORCE_EXPLICIT_CTOR)
# Require GLM to use SIMD intrinsics. Intrinsics are determined by the compiler's architecture settings (-march, /Arch)
target_compile_definitions(${PROJECT_NAME} PRIVATE GLM_FORCE_INTRINSICS)
# Force GLM to use aligned types and load/store functions (speedier SIMD ops)
target_compile_definitions(${PROJECT_NAME} PRIVATE GLM_FORCE_DEFAULT_ALIGNED_GENTYPES)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

#find_package(SDL3 REQUIRED)
#target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

# target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)

# TODO: Enable ISO C11+ Annex K safe functions like strcpy_s, sprintf_s, etc. via safeclib.
# TODO: Does MSYS2 UCRT64 already provide this? Should we replace it with this?
# TODO: conan doesn't appears to have a safeclib package. Perhaps include this in a subdir of the
# TODO: project or do a FetchContent.
#find_package(safeclib REQUIRED)
#target_link_libraries(${PROJECT_NAME} PRIVATE safeclib::safeclib)
#target_compile_definitions(${PROJECT_NAME} PRIVATE USE_SAFECLIB=1)

# TODO: Remove Vulkan from GLAD. Vulkan provides Vulkan::Vulkan target as a loader.
# GLAD is a GFX API loader generator.
# - OpenGL::GL should use the GLAD loader to load versions >1.1 plus extensions
# - Vulkan::Vulkan provides its own loader, so the GLAD loader is not required
#add_subdirectory(other/glad)
# target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)

# enable_testing()
# add_subdirectory(test)

#[[
    Grab all valid project sources in src dir and subdirs.
    Grab includes as well.
]]
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

#[[
    NASM Internal Library Target

    - Create a Static Library Target for *.asm sources and link to EXE.
    - If debug, enable correct debug format
    - Select sources and includes for NASM target, and link to main exe target
]]
add_library(${PROJECT_NAME}_Nasm STATIC)
add_library(${PROJECT_NAME}::Nasm ALIAS ${PROJECT_NAME}_Nasm)
# Enable Debug Mode Flag (-g) and debug format for NASM
if (MSVC)
    # CodeView (Windows, Visual C++)
    set_target_properties(${PROJECT_NAME}_Nasm PROPERTIES ASM_NASM_FLAGS_DEBUG "-g -Fcv8")
else()
    # Dwarf (Linux, MacOS, FreeBSD, WSL, Cygwin, MSYS2 UCRT64)
    set_target_properties(${PROJECT_NAME}_Nasm PROPERTIES ASM_NASM_FLAGS_DEBUG "-g -Fdwarf")
endif()
file(GLOB_RECURSE SOURCES_NASM CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.asm"
)
target_sources(${PROJECT_NAME}_Nasm PRIVATE ${SOURCES_NASM})
target_include_directories(${PROJECT_NAME}_Nasm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/asminc)
MESSAGE(STATUS "Created NASM Target")
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}::Nasm)

#[[
    Additional Notes
    ################
]]

# If you're making a Static or Dynamic Library Target, use tcf INTERFACE to export a minimum standard for use.
# target_compile_features(my_lib INTERFACE c_std_23)
#
# TODO: conan vs ccache

# TODO: Don't use CMAKE_BUILD_TYPE for multi-config generators like Visual Studio.
# TODO: Insetead, use generator expresisions $<$<CONFIG:Debug>:...> and $<$<CONFIG:Release>:...>
# TODO: What about NASM regarding Debug/Release?
# TODO: Prefer $<CONFIG:xxx> gens for target_ functions, see if this works
# TODO: For multiple checks, see $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Debug>>:-fno-omit-frame-pointer>
# TODO: Also see, ASM_NASM_FLAGS_DEBUG "$<$<CONFIG:Debug>:-g -Fdwarf>"
# TODO: Fix issue in CLion regarding NASM Debugging