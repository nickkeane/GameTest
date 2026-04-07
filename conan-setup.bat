@echo off
conan install . -pr:a=conan-profiles/ucrt64-debug -of conan-build-ucrt64 --build=missing
conan install . -pr:a=conan-profiles/ucrt64-release -of conan-build-ucrt64 --build=missing
conan install . -pr:a=conan-profiles/vs2026-debug -of conan-build-vs2026-debug --build=missing
conan install . -pr:a=conan-profiles/vs2026-release -of conan-build-vs2026-release --build=missing