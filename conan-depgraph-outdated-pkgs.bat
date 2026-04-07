@echo off
conan graph outdated -pr conan-profiles/ucrt64-debug -f json . >graph-outdated.json