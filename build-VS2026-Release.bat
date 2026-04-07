@echo off
rm -rf build
cmake -S . -B build/vs2026/Release -G "Visual Studio 18 2026"
cmake --build build/vs2026/Release --config Release --parallel 32