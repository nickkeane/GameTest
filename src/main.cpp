#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>

#include "Random.h"

// Ensure we have the expected floating-point support and that fast math is forced
static_assert(std::numeric_limits<float>::is_iec559,
              "IEEE 754 float support required");
static_assert(sizeof(float) == 4, "32-bit float support required");

static_assert(std::numeric_limits<double>::is_iec559,
              "IEEE 754 double support required");
static_assert(sizeof(double) == 8, "64-bit double support required");

#ifdef _MSC_VER  // MSBuild
#  ifndef _M_FP_FAST
#    warning "FP Fast is disabled"
#  endif
#else  // Not MSBuild
#  ifndef __FAST_MATH__
#    warning "FP Fast is disabled"
#  endif
#endif

//#include <zlib.h>

#include <fmt/core.h>
#include <simdjson.h>
#include <boost/asio/io_context.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/coroutine2/all.hpp>

//#include <nlohmann/json.hpp>
//#include <simdjson.h> - Faster and more modern option

//#include "Game.hpp"

// Example: NASM-compiled Assembly Math Functions
// Include maps to global exports from .asm files in src/nasm/*
#include <nasm/math.h>

int main(int const argc, const char **argv) {
  try {
    // TODO: VSCode is having issues finding fmt/core.h as well as Random.h, even though
    // TODO: they are included via conan and the target include directories.
    // TODO: C++ and C standards appear to not be getting set correctly in VSCode, see #include <concepts>
    // TODO: Clang-Format in VSCode is barfing out YAML:5:18: error: unknown key 'QualifierOrder'
    // TODO: QualifierOrder: ['type', 'const', 'volatile']
    (void)argc;
    (void)argv;

    int64_t val;
    val = nasmAdd(5LL, 10LL);
    fmt::print("Result: {0}\n", val);
    val = nasmSub(66LL, 8LL);
    fmt::print("Result: {0}\n", val);
    val = nasmIMul(10LL, 10LL);
    fmt::print("Result: {0}\n", val);
    val = nasmIDiv(100LL, 10LL);
    fmt::print("Result: {0}\n", val);

    NK::Random& rndEngine = NK::Random::getInstanceForThread();
    //NK::Random& rndEngineGlobalEdition = NK::Random::getInstanceGlobal();
    rndEngine.init();
    // Something is going wrong here
    NK::RandomSource<uint64_t> rndSource(rndEngine.getEngine(), 1, 50);
    fmt::print("Random Val: {0} seconds", rndSource());

    simdjson::builder::string_builder strBuilder;
    //simdjson::builder jsonBuilder;
    strBuilder.start_object();
    strBuilder.append_key_value("message", "Hello from Conan & CMake!");
    strBuilder.end_object();

    //  std::string jsonString = jsonBuilder.finalize();

    // nlohmann::json jsonObj;
    // jsonObj["message"] = "Hello from Conan & CMake!";
    // fmt::print("{}\n", jsonObj["message"].get<std::string>());
    //int64_t nNasmRet = nasm_add(55, 30);
    //fmt::print("Result: {0}\n", nNasmRet);
  } catch (std::exception const &e) {
    fmt::print("An exception has occurred: {0}", e.what());
  }

  return EXIT_SUCCESS;
}