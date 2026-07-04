cmake_minimum_required(VERSION 4.2 FATAL_ERROR)

#[[
    Policy Controls
]]
# Enforce that ASM (MASM) requests in external dependencies for MSVC explicitly
# use ml.exe/ml64.exe (MASM Assembler) and not cl.exe (C/C++ Compiler/Linker)
if(POLICY CMP0194)
    cmake_policy(SET CMP0194 NEW)
endif()

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
    Project Settings
]]
option(PROJOPT_ENABLE_TESTING "Enable testing with Catch2" OFF)
option(PROJOPT_ENABLE_CCACHE "Enable ccache for faster rebuilds" ON)

set(PROJVAR_WINVER 0x0A00 CACHE STRING "Windows version macro value" FORCE) # Windows 10
set(PROJVAR_NTDDKVER 0x0A000010 CACHE STRING "NTDDI version macro value" FORCE) # Windows 11 24H2 | GE "Germanium"

#[[
    CMake Includes
]]
include(CheckSymbolExists)
include(CheckIPOSupported)
include(cmake/CPM.cmake)

#[[
    CCache Settings
]]
if (PROJOPT_ENABLE_CCACHE)
    find_program(CCACHE_PROGRAM ccache)
    if (CCACHE_PROGRAM)
        message(STATUS "ccache found: ${CCACHE_PROGRAM}")
        set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    else()
        message(WARNING "ccache not found")
    endif()
endif()

#[[
    CPM Settings
]]
if(DEFINED ENV{HOME})
    set(USER_HOME "$ENV{HOME}")
elseif(DEFINED ENV{USERPROFILE})
    set(USER_HOME "$ENV{USERPROFILE}")
else()
    message(FATAL_ERROR "Could not determine the home directory")
endif()
# TODO: Set CPM Cache to be a global user directory, so all CMake projects can reference the CPM generated packages
# set(CPM_SOURCE_CACHE "${USER_HOME}/.CPM/cache" CACHE PATH "Cache CPM Package Builds Here")

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
    MSYS2 Select Compatability Layer Environment

    - Since CMake MINGW represents all MSYS2 environs, we need to be more discerning
    - Check the compiler path to determine environment in use
]]
if (WIN32 AND NOT MSVC AND NOT CYGWIN)
    if (CMAKE_CXX_COMPILER MATCHES "ucrt64")
        set(UCRT64 ON)
        message(STATUS "MSYS2 GNU GCC Toolset detected: Enabling UCRT64 Windows compatability layer")
    elseif(CMAKE_CXX_COMPILER MATCHES "clang64")
        set(CLANG64 ON)
        message(STATUS "MSYS2 LLVM Clang Toolset detected: Enabling CLANG64 Windows compatability layer")
    elseif(CMAKE_CXX_COMPILER MATCHES "mingw64")
        message(FATAL_ERROR "MSYS2 GNU GCC Toolset detected: MinGW64 is obsolete, use UCRT64 or CLANG64 instead")
    elseif(CMAKE_CXX_COMPILER MATCHES "mingw32|clang32|ucrt32")
        message(FATAL_ERROR "MSYS2 32-bit toolsets are not supported: ${CMAKE_CXX_COMPILER}")
    else()
        message(FATAL_ERROR "Unsupported compiler for MSYS2: ${CMAKE_CXX_COMPILER}")
    endif()
endif()

#[[
    Ensure CPM is using cpm-package-lock.cmake
]]
CPMUsePackageLock(cpm-package-lock.cmake)

#[[
    Check for IPO/LTO Support

    Interprocedural Optimization (IPO) or Link Time Optimization (LTO) can provide significant performance
    improvements for Release builds by enabling optimizations across translation units.
    However, not all compilers and platforms support IPO/LTO, so check for support before enabling it.
]]
check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT IPO_ERROR)
if (IPO_SUPPORTED)
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        message(STATUS "IPO/LTO is supported and will be enabled for this Release build")
    else()
        message(STATUS "IPO/LTO is supported but will not be enabled for Debug builds")
    endif()
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
    - Set Global ISO C and C++ Standards to 2023 (For Dependencies Baseline)
    - Set Target ISO C and C++ Standards to 2023
    - Require strict adherence to the ISO Standards from toolsets
    - Disable custom C/C++ StdLib extensions (f.e. Use stdYY not gnuYY)
]]
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(CMAKE_C_STANDARD 23)
    set(CMAKE_C_STANDARD_REQUIRED ON)
    set(CMAKE_C_EXTENSIONS OFF)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

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
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    # Win32 Application - Not Needed with SDL3
    # if (WIN32)
    #   target_link_options(${PROJECT_NAME} PRIVATE -mwindow)
    # endif()

    # (Windows Only)
    # Enable UNICODE support for MSYS2 UCRT64 or CLANG64 linkers
    # Allows MSYS2 code to use UNICODE-based strings, enabling
    # SDL3_main Callbacks to properly create WinMain(), WndProc(),
    # and any UNICODE Win32 API Calls + CRT Calls properly
    #
    # If UNICODE and _UNICODE defs are not passed to the project,
    # this flag should not be used.
    if (WIN32)
        target_link_options(${PROJECT_NAME} PRIVATE -municode)
    endif()

    # Enable output of all warnings + extra + pedantic warnings
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wextra)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wpedantic)
    # Convert all warnings to errors. Enable with caution, as some warnings may be benign or intentional
    #target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
    # Explicit FP Fast Mode - Usually defaults to FP Precise in GNU GCC or Clang
    target_compile_options(${PROJECT_NAME} PRIVATE -ffast-math)
    # Injects canary values into the stack to detect malicious buffer overflows at function return
    target_compile_options(${PROJECT_NAME} PRIVATE -fstack-protector-strong)

    # Debug GCC/Clang Targets
    # Define _DEBUG for debug builds for GCC/Clang. MSVC provides _DEBUG for Debug Builds.
    target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:_DEBUG>)
    # Enable any optimizations that do not interfere with debugging
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:-O0>)
    target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:-O0>)
    # Force managing of stack frame pointer for all functions to allow back-tracing
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:-fno-omit-frame-pointer>)
    # Enable ASan - Fast memory error detector, detects mem errors that could silently corrupt or crash the app
    # Detects Bounds, Use After Free, Double Freeing, and Mem Leak conditions
    # TODO: Cannot find -lasan in UCRT64 environment
    #target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
    #target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
    # Checks container bounds and smart pointer dereferencing
    target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:_GLIBCXX_ASSERTIONS>)

    # Release GCC/Clang Targets
    # Define NDEBUG for Release builds for GCC/Clang
    target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:NDEBUG>)
    # V3 - Target 2017 x86-64 arch baseline (SSE4.2, AVX2, F16C) (Haswell, Zen 1)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:-march=x86-64-v3>)
    # V4 - Target 2022 x86-64 arch baseline (AVX512, etc.) (Skylake, Zen 4)
    # As of 2026, not a great target for consumer hardware.
    # My 2 year old i9 laptop doesn't even have support for AVX512 lmao.
    # Don't enable this until 2035 at the earliest.
    #target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:-march=x86-64-v4>)
    # No container bounds or smart pointer dereferencing checks
    target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:_GLIBCXX_NO_ASSERTIONS>)
    # Strip Symbols from linker (Symbol Table + Debug Info)
    target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:-s>)
elseif(MSVC)
    # Win32 Application - Not Needed with SDL3
    #target_link_options(${PROJECT_NAME} PRIVATE /SUBSYSTEM:WINDOWS)

    # For Debug builds, ensure optimization is disabled
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:/Od>)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:/Zi>)

    # AVX2 - Target 2017 x86-64 arch baseline (SSE4.2, AVX2, F16C) (Haswell, Zen 1)
    target_compile_options(${PROJECT_NAME} PRIVATE /arch:AVX2)
    # AVX512 - Target 2022 x86-64 arch baseline (AVX512) (Skylake, Zen 4)
    # Don't enable this until 2035 at the earliest
    #target_compile_options(${PROJECT_NAME} PRIVATE /arch:AVX512)
    # Warning Level 4 - Output all warnings
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
    target_compile_options(${PROJECT_NAME} PRIVATE /permissive-)
    # Enable UTF-8 Sources
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
    # Enable FP Fast Mode - Usually the default for MSVC
    target_compile_options(${PROJECT_NAME} PRIVATE /fp:fast)

    # TODO: Disable Microsoft's ISO C11 Annex K Implementation. Make sure to include safeclib headers
    # TODO: last after windows headers or C/C++ standard library headers to overwrite
    #target_compile_definitions(${PROJECT_NAME} PRIVATE __STDC_WANT_LIB_EXT1__=0)
    target_compile_definitions(${PROJECT_NAME} PRIVATE __STDC_WANT_LIB_EXT1__)
    # TODO: Disable warnings about the use of "unsafe" CRT functions (i.e. printf vs printf_s)
    # TODO: in Visual Studio. *_s functions are not universally supported and have their own
    # TODO: issues, so its better to just use a library like safeclib for all platforms
    #target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
    # Disable warnings about the use of "unsafe" C++ STL buffer overruns on functions
    # like std::copy, std::fill, std::equal, etc. Probably overkill to define
    #target_compile_definitions(${PROJECT_NAME} PRIVATE _SCL_SECURE_NO_WARNINGS)
else()
    message(FATAL_ERROR "Unknown compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (IPO_SUPPORTED)
        # Link Time Optimization (Inlining, Dead Code Elimination, Devirtualization, CPU Register Realloc)
        # Can also be manually set with -flto to target_compile_options() and target_link_options() for GCC.
        set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(FATAL_ERROR "IPO/LTO is not supported for Release builds")
    endif()
endif()

#[[
    Platform options and definitions
]]
if (WIN32)
    # Force Windows 10 SDK (0x0A00) Windows headers minimum
    target_compile_definitions(${PROJECT_NAME} PRIVATE WINVER=${PROJVAR_WINVER})
    target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=${PROJVAR_WINVER})
    target_compile_definitions(${PROJECT_NAME} PRIVATE NTDDI_VERSION=${PROJVAR_NTDDKVER})
    # Reduce little-used <windows.h> includes, such as GDI, Cryptography, DDE, and ShellAPI.
    target_compile_definitions(${PROJECT_NAME} PRIVATE WIN32_LEAN_AND_MEAN)
    # Use ISO C/C++ std::min/std::max instead of min and max macros, which can break some libraries.
    target_compile_definitions(${PROJECT_NAME} PRIVATE NOMINMAX)
    # Force strict Win32 API type checking (i.e. HWND vs HDC)
    # Modern compilers enable this by default, but it doesn't hurt to make explicit.
    target_compile_definitions(${PROJECT_NAME} PRIVATE STRICT)
    # Force Unicode (W) functions for Win32 API.
    #
    # Can disable if wanting to use (A) functions and don't care about internationalization.
    # Best practice is to define UNICODE and use L"" string literals and std::wstring.c_str()
    # for any Win32 API calls involving strings. Oh, and use StdLib equivalents wherever possible.
    # Old best practice is to use Win32 API function macros, the types TCHAR, LPTSTR, and LPCTSTR
    # to manage string objects and _T() or TEXT() to convert string literals, as well as <tchar.h>
    # variants of string operations on TCHAR types i.e. _tchlen() instead of strlen()
    # MultiByteToWideChar and WideCharToMultiByte can convert strings between UNICODE and ANSI
    #
    # For MSYS2 compatibility, see -municode linker flag
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
endif()

#[[
    External Libraries
    The Conan and CMake profiles provide the correct paths for external
    dependencies for specific build configs (UCRT64, VS2026 and Debug, Release)
]]
# (Required) Threads
## - Find Native Threads for std::thread support in C++11 and later
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
# (Optional) CUDA Toolkit + CCCL
find_package(CUDAToolkit)
if (CUDAToolkit_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE CUDA::cudart)
    CPMAddPackage("gh:nvidia/cccl@3.3.4")
    target_link_libraries(${PROJECT_NAME} PRIVATE CCCL::CCCL)
endif()
# (Optional) OpenMP for CPU Parallelism + GPU Compute via CUDA
find_package(OpenMP)
if (OpenMP_FOUND)
    if (OpenMP_C_FOUND)
        target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_C)
    endif()
    if (OpenMP_CXX_FOUND)
        target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)
    endif()
    if (OpenMP_CUDA_FOUND)
        target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CUDA)
    endif()
endif()
# (Optional) OpenCL for GPU Compute
find_package(OpenCL)
if (OpenCL_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::OpenCL)
endif()
if (WIN32)
    # Windows / MSYS2 UCRT64 / Cygwin
    find_package(OpenGL REQUIRED)
    find_package(Vulkan REQUIRED COMPONENTS volk)
    CPMAddPackage("gh:microsoft/DirectX-Headers@1.721.0-preview")
    #set(LLVM_TARGETS_TO_BUILD "X86" "AMDGPU" "NVPTX" CACHE STRING "LLVM targets to build")
    #CPMAddPackage("gh:microsoft/directxshadercompiler@1.10.2605.24")
    # SDL3 or GLFW should be used for windowing and input rather than raw Win32 API calls
    # Winsock 2 for WSAStartup and WSACleanup pre-reqs for asio on Win32
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
    # Microsoft-specific Winsock extensions (AcceptEx, ConnectEx, DisconnectEx, etc.)
    # Used by asio for high-perf async networking on Win32
    target_link_libraries(${PROJECT_NAME} PRIVATE mswsock)

    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Vulkan::Vulkan
            Vulkan::Headers
            Vulkan::volk
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Microsoft::DirectX-Headers
            Microsoft::DirectX-Guids
    )
    # XInput and DirectInput are not required if using SDL3 or a seperate
    # 3rd party input library that does the same functionality
    target_link_libraries(${PROJECT_NAME} PRIVATE xinput)
    target_link_libraries(${PROJECT_NAME} PRIVATE dxguid)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DIRECTINPUT_VERSION=0x0800)
    target_link_libraries(${PROJECT_NAME} PRIVATE dinput8)
    target_link_libraries(${PROJECT_NAME} PRIVATE d3d12)
    target_link_libraries(${PROJECT_NAME} PRIVATE dxgi)
elseif (UNIX AND NOT APPLE)
    # Linux
    # OpenGL + Linux X11 Windowing + ALSA Audio
    find_package(X11 REQUIRED)
    set(OpenGL_GL_PREFERENCE "GLVND" CACHE STRING "Use GL Vendor-Neutral Dispatch for Linux" FORCE)
    find_package(OpenGL REQUIRED)
    find_package(Vulkan REQUIRED COMPONENTS volk)
    find_package(ALSA REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE dl)
    # May already be handled by Threads::Threads. Enable if not.
    #target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
    target_link_libraries(${PROJECT_NAME} PRIVATE X11)
    if (OpenGL_OpenGL_FOUND)
        target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::OpenGL OpenGL::GLX)
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
    endif()
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Vulkan::Vulkan
            Vulkan::Headers
            Vulkan::volk
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE ALSA::ALSA)
elseif(APPLE)
    # MacOS
    # OpenGL Core v4.x or Vulkan + MoltenVK or Metal
    find_package(OpenGL REQUIRED)
    find_package(Vulkan REQUIRED COMPONENTS MoltenVK volk)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            "-framework CoreFoundation"
            "-framework CoreVideo"
            "-framework QuartzCore"
            "-framework IOKit"
            "-framework Cocoa"
            "-framework OpenGL"
            "-framework Metal"
            "-framework MetalKit"
            Vulkan::Vulkan
            Vulkan::Headers
            Vulkan::MoltenVK
            Vulkan::volk
    )
elseif(ANDROID)
    # Android
    # OpenGLES v3 or Vulkan
    find_package(Vulkan REQUIRED COMPONENTS volk)
    target_link_libraries(${PROJECT_NAME} PRIVATE android)
    target_link_libraries(${PROJECT_NAME} PRIVATE log)
    target_link_libraries(${PROJECT_NAME} PRIVATE GLESv3)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Vulkan::Vulkan
            Vulkan::Headers
            Vulkan::volk
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

# Vulkan-Hpp
# Object-Oriented Vulkan for C++
if (WIN32 OR UNIX OR ANDROID)
    CPMAddPackage("gh:KhronosGroup/Vulkan-Hpp@1.4.352")
    target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Hpp)
endif()

##[[
#    Utilize safeclib
#    TODO: safeclib does not provide CMake builds currently. See https://github.com/rurban/safeclib/issues/137.
#]]
## TODO: Enable ISO C11+ Annex K safe functions like strcpy_s, sprintf_s, etc. via safeclib.
## TODO: Does MSYS2 UCRT64 already provide this? Should we replace it with this?
## TODO: conan doesn't appears to have a safeclib package. Perhaps include this in a subdir of the
## TODO: project or do a FetchContent.
## safeclib - ISO C11 Annex K Multiplatform Safe C Library Functions (i.e. strcpy_s, sprintf_s, etc.)
#CPMAddPackage("gh:rurban/safeclib@31082020")
#target_compile_definitions(${PROJECT_NAME} PRIVATE SAFECLIB_DISABLE_STDC_WANT_SECURE_LIB)
#target_compile_definitions(${PROJECT_NAME} PRIVATE USE_SAFECLIB=1)
#target_link_libraries(${PROJECT_NAME} PRIVATE safeclib::safeclib)

#[[
    Find Packages, and Link Libraries

    - Ensure commonly-used headers are precompiled for faster builds
    - Packages are provided by CMake Package Manager (CPM)
]]
## TODO: Enable OpenSSL CMake target for project use and subproject use
## TODO: for HTTPS (asio, uwebsockets, minicoro, stdexec, etc.)
## OpenSSL - TLS and Crypto Library
#find_package(OpenSSL)
#if (OpenSSL_FOUND)
#    target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)
#endif()
## libunwind - Call Chain Tracing for Backtraces and Debugging
#if (UNIX AND NOT APPLE)
#    CPMAddPackage("gh:libunwind/libunwind@1.8.3")
#    target_link_libraries(${PROJECT_NAME} PRIVATE libunwind::libunwind)
#endif()

# abseil - Highly Optimized STL Libraries from Google
CPMAddPackage("gh:abseil/abseil-cpp#20260526.0")
target_link_libraries(${PROJECT_NAME} PRIVATE 
    absl::base              # Base Library (absl::base_internal, absl::base_sync, etc.)
    absl::status            # Status Library (absl::Status, absl::StatusOr, absl::OkStatus, etc.)
    absl::utility           # Utility Library (absl::make_unique, absl::make_shared, absl::move_only_function, etc.)

    absl::algorithm         # Algorithm Library (absl::c_find_if, absl::c_sort, absl::c_binary_search, etc.)
    # Containers
    absl::fixed_array       # Fixed-Size Array Library (absl::fixed_array, absl::make_fixed_array, etc.)
    absl::flat_hash_map     # Flat Hash Map Library (absl::flat_hash_map, absl::flat_hash_set, etc.)
    absl::flat_hash_set     # Flat Hash Set Library (absl::flat_hash_set, absl::flat_hash_map, etc.)
    absl::btree             # B-Tree Library (absl::btree_map, absl::btree_set, etc.)
    absl::node_hash_map     # Node Hash Map Library (absl::node_hash_map, absl::node_hash_set, etc.)
    absl::node_hash_set     # Node Hash Set Library (absl::node_hash_set, absl::node_hash_map, etc.)
    #
    absl::debugging         # Debugging Library (absl::DebugString, absl::DumpBacktrace, etc.)
    absl::flags             # Commandline Flags Library (absl::Flag, absl::ParseCommandLine, etc.)
    absl::hash              # Hash Library (absl::Hash, absl::HashState, etc.)
    absl::memory            # Memory Library (absl::make_unique, absl::make_shared, absl::InlinedVector, etc.)
    absl::meta              # Meta Programming Library (absl::enable_if_t, absl::void_t, absl::is_invocable, etc.)
    absl::random_random     # Random Library (absl::BitGen, absl::Uniform, absl::BernoulliDistribution, etc.)
    absl::strings           # String Library (absl::StrCat, absl::StrFormat, absl::StrSplit, absl::StrJoin, etc.)
    absl::synchronization   # Synchronization Library (absl::Mutex, absl::CondVar, absl::Notification, etc.)
    absl::time              # Time Library (absl::Time, absl::Duration, absl::TimeZone, etc.)
)
# stb - Alternative STL Libraries
CPMAddPackage(
    NAME stb
    GITHUB_REPOSITORY nothings/stb
    GIT_TAG master
    DOWNLOAD_ONLY YES
)
if (stb_ADDED)
    add_library(stb INTERFACE)
    add_library(stb::stb ALIAS stb)

    target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

    set(stb_FOUND YES CACHE BOOL "STB was found and generated" FORCE)

    message(STATUS "CPM: stb::stb assembled as header-only INTERFACE target")
else()
    message(FATAL_ERROR "CPM: Failed to create stb package")
endif()
if (stb_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE stb::stb)
endif()
# mimalloc - Better Memory Allocator from Microsoft
set(MI_BUILD_TESTS OFF CACHE BOOL "Disable building of mimalloc tests" FORCE)
set(MI_OVERRIDE ON CACHE BOOL "Override default malloc/new/delete with mimalloc" FORCE)
set(MI_OPT_ARCH ON CACHE BOOL "Enable Architecture Opts" FORCE)
set(MI_OPT_SIMD ON CACHE BOOL "Use SIMD Intrinsics" FORCE)
CPMAddPackage("gh:microsoft/mimalloc@3.3.2")
target_link_libraries(${PROJECT_NAME} PRIVATE mimalloc)
# pcg - Random Number Generation (Use instead of std::mt19937 or std::rand)
set(PCG_CPP_BUILD_SAMPLES OFF CACHE BOOL "Don't build PCG Samples" FORCE)
set(PCG_CPP_BUILD_TESTS OFF CACHE BOOL "Don't build PCG Tests" FORCE)
CPMAddPackage("gh:Total-Random/pcg-cpp@1.0")
target_link_libraries(${PROJECT_NAME} PRIVATE pcg_cpp::pcg_cpp)
# CPMAddPackage(
#         NAME pcg
#         GITHUB_REPOSITORY imneme/pcg-cpp
#         GIT_TAG master
#         DOWNLOAD_ONLY YES
# )
# if (pcg_ADDED)
#     add_library(pcg INTERFACE)
#     add_library(pcg::pcg ALIAS pcg)

#     target_include_directories(pcg INTERFACE ${pcg_SOURCE_DIR}/include)

#     set(pcg_FOUND YES CACHE BOOL "PCG-CPP was found and generated" FORCE)

#     message(STATUS "CPM: pcg::pcg assembled as header-only INTERFACE target")
# else()
#     message(FATAL_ERROR "CPM: Failed to create pcg package")
# endif()
# if (pcg_FOUND)
#     target_link_libraries(${PROJECT_NAME} PRIVATE pcg::pcg)
# endif()
# zlib - Compression/Decompression
if (NOT MSVC AND NOT ANDROID) # SDL3_image provides vendored ZLIB if Android or MSVC is detected
    set(ZLIB_BUILD_TESTING OFF CACHE BOOL "Disable building of zlib tests" FORCE)
    CPMAddPackage("gh:madler/zlib@1.3.2")
    target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)
endif()
# lz4 - Compression/Decompression
CPMAddPackage(
    NAME lz4
    GITHUB_REPOSITORY lz4/lz4
    VERSION 1.10.0
    DOWNLOAD_ONLY YES
)
if (lz4_ADDED)
    #set(BUILD_SHARED_LIBS OFF)
	#set(BUILD_STATIC_LIBS ON)
	set(LZ4_BUILD_CLI OFF)
    add_subdirectory(${lz4_SOURCE_DIR}/build/cmake lz4_build)
    target_link_libraries(${PROJECT_NAME} PRIVATE lz4)
else()
    message(FATAL_ERROR "CPM: Failed to create lz4 package")
endif()
# fmt - Text Formatting Utility
CPMAddPackage("gh:fmtlib/fmt#12.1.0")
target_precompile_headers(${PROJECT_NAME} PRIVATE
        <fmt/core.h>
        <fmt/format.h>
)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
# spdlog - Logging Utility
set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "Use external fmt library for spdlog instead of bundled" FORCE)
CPMAddPackage("gh:gabime/spdlog@1.17.0")
target_precompile_headers(${PROJECT_NAME} PRIVATE
        <spdlog/spdlog.h>
        <spdlog/fmt/fmt.h>
)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
## SIMDJson - JSON Parser
#set(SIMDJSON_DEVELOPER_MODE OFF CACHE BOOL "Disable SIMDJSON developer mode to prevent global compiler option #overrides" FORCE)
#CPMAddPackage("gh:simdjson/simdjson@4.6.4")
#target_link_libraries(${PROJECT_NAME} PRIVATE simdjson::simdjson)
## RapidYAML - YAML Parser
#CPMAddPackage("gh:biojppm/rapidyaml@0.13.0")
#target_link_libraries(${PROJECT_NAME} PRIVATE ryml::ryml)
## tomlplusplus - TOML Parser
#set(TOMLPLUSPLUS_BUILD_MODULES ON CACHE BOOL "Enable C++20 modules for tomlplusplus" FORCE)
#CPMAddPackage("gh:marzer/tomlplusplus@3.4.0")
#target_link_libraries(${PROJECT_NAME} PRIVATE tomlplusplus::tomlplusplus)
# Catch2 - Testing
set(CATCH_INSTALL_DOCS OFF CACHE BOOL "Disable installation of Catch2 documentation" FORCE)
set(CATCH_INSTALL_EXTRAS OFF CACHE BOOL "Disable installation of Catch2 extras" FORCE)
set(CATCH_DEVELOPMENT_BUILD OFF CACHE BOOL "Disable development build features in Catch2" FORCE)
CPMAddPackage("gh:catchorg/Catch2@3.15.0")
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2)
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2WithMain)
# sfl - C++11 Containers (Vectors + Maps) (Use alongside absl::containers)
CPMAddPackage("gh:slavenf/sfl-library#2.2.0")
target_link_libraries(${PROJECT_NAME} PRIVATE sfl::sfl)
# CLI11 - Command-line Parser for C++11 and later (Use instead of absl::flags)
set(CLI11_PRECOMPILED ON CACHE BOOL "Enable precompiled headers for CLI11" FORCE)
set(CLI11_MODULES ON CACHE BOOL "Enable C++20 modules for CLI11" FORCE)
CPMAddPackage("gh:CLIUtils/CLI11@2.6.2")
target_precompile_headers(${PROJECT_NAME} PRIVATE
        <CLI/CLI.hpp>
)
target_link_libraries(${PROJECT_NAME} PRIVATE CLI11::CLI11)
# fixed_math - High Performance C++23 Fixed-Point (48.16) Math Library
CPMAddPackage("gh:arturbac/fixed_math@2.2.0")
target_precompile_headers(${PROJECT_NAME} PRIVATE
        <fixedmath/fixed_math.hpp>
)
target_link_libraries(${PROJECT_NAME} PRIVATE fixed_math)
# GLM - Vector/Matrix Math Library
set(GLM_ENABLE_CXX_20 ON CACHE BOOL "Enable C++20 support for GLM" FORCE)
CPMAddPackage("gh:g-truc/glm#1.0.3")
## Force GLM to use explicit constructors for type conversions (i.e. ivec4 a; vec4 b = a; would be an error. Use vec4 b(a) instead).
target_compile_definitions(${PROJECT_NAME} PRIVATE GLM_FORCE_EXPLICIT_CTOR)
## Require GLM to use SIMD intrinsics. Intrinsics are determined by the compiler's architecture settings (-march, /Arch)
target_compile_definitions(${PROJECT_NAME} PRIVATE GLM_FORCE_INTRINSICS)
## Force GLM to use aligned types and load/store functions (speedier SIMD ops)
target_compile_definitions(${PROJECT_NAME} PRIVATE GLM_FORCE_DEFAULT_ALIGNED_GENTYPES)
target_precompile_headers(${PROJECT_NAME} PRIVATE
        <glm/vec3.hpp>
        <glm/vec4.hpp>

        <glm/mat3x3.hpp>
        <glm/mat4x4.hpp>

        <glm/gtc/matrix_transform.hpp>

        <glm/gtc/quaternion.hpp>
)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
# ASIO - Asynchronous I/O and Networking Library
## ASIO does not provide CMakeLists.txt, so assemble a header-only INTERFACE target for it.
CPMAddPackage(
        NAME asio
        GITHUB_REPOSITORY chriskohlhoff/asio
        GIT_TAG asio-1-38-0
        DOWNLOAD_ONLY YES
)
if (asio_ADDED)
    add_library(asio INTERFACE)
    add_library(asio::asio ALIAS asio)

    target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/include)
    target_compile_definitions(asio INTERFACE ASIO_STANDALONE)
    # Allow ASIO to work with WinSock2
    if(WIN32)
        target_compile_definitions(asio INTERFACE _WIN32_WINNT=${PROJVAR_WINVER})
        target_link_libraries(asio INTERFACE ws2_32)
        target_link_libraries(asio INTERFACE mswsock)
    endif()

    # set(ASIO_INCLUDE_DIR "${asio_SOURCE_DIR}/include" CACHE PATH "Asio include directory" FORCE)
    set(ASIO_STANDALONE ON CACHE BOOL "Define ASIO_STANDALONE for header-only use" FORCE)
    set(asio_FOUND YES CACHE BOOL "ASIO was found and generated" FORCE)

    message(STATUS "CPM: asio::asio assembled as header-only INTERFACE target")
else()
    message(FATAL_ERROR "CPM: Failed to create asio package")
endif()
if (asio_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE asio::asio)
endif()
## stdexec - TODO: Can be removed when using C/C++26 standard
#set(STDEXEC_BUILD_TESTS OFF CACHE BOOL "Disable building of stdexec tests" FORCE)
#CPMAddPackage("gh:nvidia/stdexec#nvhpc-25.09")
#target_link_libraries(${PROJECT_NAME} PRIVATE STDEXEC::stdexec)
## minicoro - Stackful Coroutines
### Just a header file. Assemble into a header-only INTERFACE target.
#CPMAddPackage("gh:edubart/minicoro@0.1.3")
#if (minicoro_ADDED)
#    add_library(minicoro INTERFACE)
#    add_library(minicoro::minicoro ALIAS minicoro)
#
#    target_include_directories(minicoro INTERFACE ${minicoro_SOURCE_DIR})
#
#    set(minicoro_FOUND ON CACHE BOOL "minicoro was found and assembled" FORCE)
#
#    message(STATUS "CPM: minicoro::minicoro assembled as header-only INTERFACE target")
#else()
#    message(FATAL_ERROR "CPM: Failed to create minicoro package")
#endif()
#if (minicoro_FOUND)
#    target_link_libraries(${PROJECT_NAME} PRIVATE minicoro::minicoro)
#endif()
## ctre - Compile Time Regular Expressions
#set(CTRE_MODULE OFF CACHE BOOL "Disable C++20 modules for CTRE, as issues are occurring" FORCE)
#CPMAddPackage("gh:hanickadot/compile-time-regular-expressions@3.11.0")
#target_link_libraries(${PROJECT_NAME} PRIVATE ctre)
## sigslot - C++14 Signal/Slot Library
#CPMAddPackage("gh:palacaze/sigslot@1.2.3")
#target_link_libraries(${PROJECT_NAME} PRIVATE Pal::Sigslot)
# # cista - Serialization Library (Potentially faster than Flatbuffers, but could also be buggier)
# set(CISTA_USE_MIMALLOC ON CACHE BOOL "Enable mimalloc support" FORCE)
# CPMAddPackage("gh:felixguendling/cista@0.16")
# target_link_libraries(${PROJECT_NAME} PRIVATE cista::cista)
# flatbuffers - Serialization Library by Google for GameDev
set(FLATBUFFERS_BUILD_TESTS OFF CACHE BOOL "Disable building of Flatbuffers tests" FORCE)
CPMAddPackage("gh:google/flatbuffers@25.12.19")
target_link_libraries(${PROJECT_NAME} PRIVATE FlatBuffers::FlatBuffers)
## crow - HTTP/Web Socket Microframework
#set(CROW_INSTALL OFF CACHE BOOL "Disable installation of Crow" FORCE) # Breaks on Project asio
#set(CROW_ENABLE_SSL OFF CACHE BOOL "Enable SSL support (HTTPS) for Crow" FORCE) # Requires OpenSSL
#set(CROW_ENABLE_COMPRESSION ON CACHE BOOL "Enable HTTP compression support for Crow" FORCE)
#CPMAddPackage("gh:crowcpp/crow@1.3.2")
#target_link_libraries(${PROJECT_NAME} PRIVATE Crow::Crow)
## xxHash - Fast Non-Cryptographic Hash Algorithm
#set(XXHASH_BUILD_XXHSUM ON CACHE BOOL "Disable xxHash command line exe generation" FORCE)
#CPMAddPackage("gh:Cyan4973/xxHash#e573d4d")
#if (xxHash_ADDED)
#    add_subdirectory(${xxHash_SOURCE_DIR}/build/cmake)
#    if (TARGET xxHash::xxhash)
#        target_link_libraries(${PROJECT_NAME} PRIVATE xxHash::xxhash)
#    else()
#        MESSAGE(FATAL_ERROR "Expected xxHash::xxhash target does not exist")
#    endif()
#endif()
# nameof - C++11 Reflection for Types
CPMAddPackage("gh:Neargye/nameof@0.10.5")
target_link_libraries(${PROJECT_NAME} PRIVATE nameof::nameof)
# magic_enum - C++11 Reflection for Enums
CPMAddPackage("gh:Neargye/magic_enum@0.9.8")
target_link_libraries(${PROJECT_NAME} PRIVATE magic_enum::magic_enum)
## box2d - 2D Physics Engine
## joltphysics - 2D/3D Physics Engine
## flecs - Entity System
#CPMAddPackage("gh:sandermertens/flecs@4.1.5")
#target_link_libraries(${PROJECT_NAME} PRIVATE flecs::flecs)
# EnTT - Entity System
CPMAddPackage("gh:skypjack/entt@3.16.0")
target_link_libraries(${PROJECT_NAME} PRIVATE EnTT::EnTT)
# glad - GL + Vulkan Extension Loader
## TODO: Remove Vulkan from GLAD. Vulkan provides Vulkan::Vulkan target as a loader.
## TODO: Or should we use Vulkan with GLAD. Do more research on this. Use Vulkan::volk? Or just Vulkan::Vulkan?
## GLAD is a GFX API loader generator.
## - OpenGL::GL should use the GLAD loader to load versions >1.1 plus extensions
## - Vulkan::Vulkan provides its own loader, so the GLAD loader is not required
## - Use https://gen.glad.sh/ to generate (gl >=4.6 Core + vulkan >=1.4 + Add All Extensions + alias + loader).
## - Enable debug for glad_debug folder, disable debug for glad_release. Provides extra error checking for gl calls.
## - Contrary to its claim, debug does not generate a "debug build" of GLAD. Rather, it wraps GL/VK calls with extra error checking.
## - Include merge option to combine multiple APIs in one include <glad/glad.h> (not recommended).
## - Include mx option for multi-context GL support, required for multi-window apps (uncommon).
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(other/glad-debug)
else ()
    add_subdirectory(other/glad-release)
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)
# # TODO: Clay - UI Layout Library
# # microui - Micro UI Library
# CPMAddPackage(
#         NAME microui
#         GITHUB_REPOSITORY rxi/microui
#         GIT_TAG master
#         DOWNLOAD_ONLY YES
# )
# if (microui_ADDED)
#     add_library(microui ${microui_SOURCE_DIR}/src/microui.c)
#     add_library(microui::microui ALIAS microui)
#     target_include_directories(microui PUBLIC ${microui_SOURCE_DIR}/src)

#     set(microui_FOUND YES CACHE BOOL "microui was found and generated" FORCE)

#     message(STATUS "CPM: microui::microui assembled as static library target")
# else()
#     message(FATAL_ERROR "CPM: Failed to create microui package")
# endif()
# ImGUI - Overlay Immediate Mode GUI Library
CPMAddPackage(
        NAME imgui
        GITHUB_REPOSITORY ocornut/imgui
        GIT_TAG v1.92.8
        DOWNLOAD_ONLY YES
)
if (imgui_ADDED)
    add_library(imgui INTERFACE)
    add_library(imgui::imgui ALIAS imgui)
    target_include_directories(imgui INTERFACE ${imgui_SOURCE_DIR})

    add_library(imgui_backends INTERFACE)
    add_library(imgui::backends ALIAS imgui_backends)
    target_include_directories(imgui_backends INTERFACE ${imgui_SOURCE_DIR}/backends)

    set(imgui_FOUND YES CACHE BOOL "imgui was found and generated" FORCE)

    message(STATUS "CPM: imgui::imgui assembled as header-only INTERFACE target")
else()
    message(FATAL_ERROR "CPM: Failed to create imgui package")
endif()
if (imgui_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui imgui::backends)
endif()
# FreeType - Font Rendering Library
if (NOT MSVC AND NOT ANDROID) # SDL3_image provides vendored Freetype if Android or MSVC is detected
    find_package(Freetype)
    if (NOT Freetype_FOUND)
        CPMAddPackage("gh:freetype/freetype#VER-2-14-3")
    endif()
    if(NOT TARGET Freetype::Freetype)
        add_library(Freetype::Freetype ALIAS freetype)
    endif()
    target_link_libraries(${PROJECT_NAME} PRIVATE Freetype::Freetype)
endif()
# SDL3 - Simple Direct Media Layer for Windowing, Input, Audio, and more
set(SDL_TEST_LIBRARY OFF CACHE BOOL "Disable Test Library" FORCE)
set(BUILD_DOCS OFF CACHE BOOL "Disable building of SDL_mixer/external/flac docs" FORCE)
CPMAddPackage("gh:libsdl-org/SDL#release-3.4.10")
# TODO: The below vendored check doesn't seem to be working for VS2026...
#set(SDL3_IMAGE_ZLIB OFF CACHE BOOL "" FORCE)
#set(SDL_IMAGE_VENDORED OFF CACHE BOOL "Disable ZLIB vendoring" FORCE)
#set(SDL3_IMAGE_VENDORED OFF CACHE BOOL "Disable ZLIB vendoring" FORCE)
#set(SDLIMAGE_ZLIB_VENDORED OFF CACHE BOOL "Disable ZLIB vendoring" FORCE)
CPMAddPackage("gh:libsdl-org/SDL_image#release-3.4.4")
#set(SDLTTF_FREETYPE_VENDORED OFF CACHE BOOL "Disable Freetype vendoring" FORCE)
CPMAddPackage("gh:libsdl-org/SDL_ttf#release-3.2.2")
CPMAddPackage("gh:libsdl-org/SDL_mixer#release-3.2.4")
CPMAddPackage("gh:icculus/SDL_sound@3.2.0")
CPMAddPackage("gh:libsdl-org/SDL_net#release-3.2.0")
# TODO: SDL_shadercross is in development
#CPMAddPackage("gh:libsdl-org/SDL_shadercross#main")
target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3 
    SDL3_image::SDL3_image 
    SDL3_ttf::SDL3_ttf 
    SDL3_mixer::SDL3_mixer
    SDL3_sound::SDL3_sound
    SDL3_net::SDL3_net
    #SDL3_shadercross::SDL3_shadercross
)
## assimp - Asset Importer Library (Depends on ZLib or SDL3_image vendored ZLib)
## TODO: Can we prevent assimp or SDL3_image from declaring zlibstatic? How do we handle this external conflict?
set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "Disable building of ZLib for Assimp" FORCE)
CPMAddPackage("gh:assimp/assimp@6.0.5")
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)
# pybind11 - C++11 Python 3.x.x Bindings. Includes Python C API headers and libraries for embedding.
find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Embed)
set(PYBIND11_FINDPYTHON "NEW" CACHE STRING "Use new FindPython module for pybind11" FORCE)
CPMAddPackage("gh:pybind/pybind11@3.0.4")
target_link_libraries(${PROJECT_NAME} PRIVATE pybind11::embed)

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
# Copy any required DLLs (f.e. libzd.dll) to build output directory during build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
)

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

if (PROJOPT_ENABLE_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()

#[[
    Additional Notes
    ################
]]

# TODO: Can CPM _deps folders be moved out of build directory? Should we move this to a home directory?
# TODO: Need to figure out how to allow different CMake binaries to be used for MSYS2 UCRT64 and MSVC.