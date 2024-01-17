#pragma once

#include <util.h>
#include <arch/types.h>
#include <arch/machine/registerset.h>
#include <arch/object/structures.h>

// Set the register context in tcb with w
static inline void set_register(Tcb* thread, Register reg, u64 w) {
  thread->tcbArch.tcbContext.registers[reg] = w;
}

// Get the reigster context in tcb
static inline u64 PURE get_register(Tcb* thread, Register reg) {
  return thread->tcbArch.tcbContext.registers[reg];
}