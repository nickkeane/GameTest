@echo off
rm -rf build
cmake -S . -B build -G "Visual Studio 18 2026"
cmake --build build --config Release --parallel 24