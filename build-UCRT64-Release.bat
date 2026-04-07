@echo off
rm -rf build
REM conan install . --build=missing -s build_type=Release -s compiler.cppstd=17
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build --config Release --parallel 24