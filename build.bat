@echo off
@REM conan graph info .
@REM conan graph info -f html . >graph.html
@REM conan install . --output-folder=build --build=missing
@REM cmake --preset conan-release
@REM cmake --build --preset conan-release
@REM Dry run args (-n) and verbose (-v)
@REM cmake --build ./build -- -n 
@REM Dry run for MSBuild (VS) cmake --build ./build -- /p:WhatIf=true
@REM --graphviz=graph.dot can be added to generate a dot graph that can be converted using "dot -Tpng graph.dot -o project_structure.png"

@REM This uses the python version of conanfile to build and integrate deps w/ CMakeLists.txt
@REM conan build .