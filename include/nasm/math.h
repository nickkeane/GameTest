//
// Created by Nick on 4/4/2026.
//

#ifndef MYGAMEPROJECT_NASM_MATH_H
#define MYGAMEPROJECT_NASM_MATH_H
#include <cstdint>

#if defined(__cplusplus)
extern "C" {
#endif

std::int64_t nasmAdd(std::int64_t a, std::int64_t b);
std::int64_t nasmSub(std::int64_t a, std::int64_t b);
std::int64_t nasmIMul(std::int64_t a, std::int64_t b);
std::int64_t nasmIDiv(std::int64_t a, std::int64_t b);

#if defined(__cplusplus)
}
#endif

#endif //MYGAMEPROJECT_NASM_MATH_H
