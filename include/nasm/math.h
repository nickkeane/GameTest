//
// Created by Nick on 4/4/2026.
//

#ifndef MYGAMEPROJECT_NMATH_H
#define MYGAMEPROJECT_NMATH_H
#include <cstdint>

#if defined(__cplusplus)
extern "C" {
#endif

int64_t nasm_add(int64_t a, int64_t b);
int64_t nasm_sub(int64_t a, int64_t b);
int64_t nasm_mul(int64_t a, int64_t b);
int64_t nasm_div(int64_t a, int64_t b);

#if defined(__cplusplus)
}
#endif

#endif //MYGAMEPROJECT_NMATH_H
