@echo off
rm -rf build
cmake --preset UCRT64-Debug
cmake --build --preset UCRT64-Debug
cmake --preset UCRT64-Release
cmake --build --preset UCRT64-Release
cmake --preset VS2026-Debug
cmake --build --preset VS2026-Debug
cmake --preset VS2026-Release
cmake --build --preset VS2026-Release