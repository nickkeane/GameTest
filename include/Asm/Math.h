//
// Created by Nick on 4/4/2026.
//

#ifndef MYGAMEPROJECT_ASM_MATH_H
#define MYGAMEPROJECT_ASM_MATH_H

#include <cstdint>

#if defined(__cplusplus)
extern "C" {
#endif

int64_t nasmAdd(int64_t a, int64_t b);
int64_t nasmSub(int64_t a, int64_t b);
int64_t nasmIMul(int64_t a, int64_t b);
int64_t nasmIDiv(int64_t a, int64_t b);

#if defined(__cplusplus)
}
#endif

#endif //MYGAMEPROJECT_ASM_MATH_H
