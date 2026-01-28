@echo off
conan install . --profile conan_profile --output-folder=build --build=missing
@REM cmake --preset conan-default
@REM cmake --preset conan-release
@REM cmake --preset conan-debug
@REM cmake --build --preset conan-default
@REM Dry run args (-n) and verbose (-v)
@REM cmake --build ./build -- -n 
@REM Dry run for MSBuild (VS) cmake --build ./build -- /p:WhatIf=true
@REM --graphviz=graph.dot can be added to generate a dot graph that can be converted using "dot -Tpng graph.dot -o project_structure.png"

@REM This uses the python version of conanfile to build and integrate deps w/ CMakeLists.txt
@REM conan build .


@REM cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
@REM cmake --build build