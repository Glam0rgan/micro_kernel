#pragma once

#include <os/config.h>
#include <os/simple_types.h>

typedef OsCPtr OsX64PML4;

typedef struct OsUserContext_t {
    OsWord rip, rsp, rflags, rax, rbx, rcx, rd, rsi, rdi, rbp, r8, r9,
        r10, r11, r12, r13, r14, r15;
    OsWord fsBase, gsBase;
}OsUserContext;