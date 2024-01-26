#pragma once

#include <config.h>
#include <arch/types.h>
#include <util.h>

#include <mode/machine/registerset.h>

#define CONFIG_XSAVE_SIZE 0
#define N_CONTEXT_REGISTERS 20

// x86 fpu context
struct UserFpuState {
  uint8_t state[CONFIG_XSAVE_SIZE];
};

// x86 user-code context
typedef struct UserContext_t {
  //UserFpuState fpuState;
  uint64_t registers[N_CONTEXT_REGISTERS];
}UserContext;

void mode_init_context(UserContext* context);
