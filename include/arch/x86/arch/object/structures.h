#pragma once

#include <config.h>
#include <util.h>
#include <api/types.h>
#include <os/macros.h>
#include <arch/types.h>
#include <arch/machine/hardware.h>
#include <arch/machine/registerset.h>
#include <os/arch/constants.h>

typedef struct ArchTcb_t {
  UserContext tcbContext;
} ArchTcb;

enum VmRights {
  VMKernelOnly = 1,
  VMReadOnly = 2,
  VMReadWrite = 3
};