#pragma once

#include <config.h>
#include <util.h>
#include <api/types.h>
#include <os/macros.h>
#include <arch/types.h>
#include <arch/machine/hardware.h>
#include <arch/machine/registerset.h>
#include <arch/machine/cpu_registers.h>
#include <os/arch/constants.h>
#include <mode/object/structures.h>

typedef struct ArchTcb_t {
  UserContext tcbContext;
} ArchTcb;

#define OS_NULL    GDT_NULL
#define OS_CS_0    (GDT_CS_0 << 3)
#define OS_DS_0    (GDT_DS_0 << 3)
#define OS_CS_3    ((GDT_CS_3 << 3) | 3)
#define OS_DS_3    ((GDT_DS_3 << 3) | 3)
#define OS_TSS     (GDT_TSS << 3)
#define OS_FS      ((GDT_FS << 3) | 3)
#define OS_GS      ((GDT_GS << 3) | 3)

enum VmRights {
  VMKernelOnly = 1,
  VMReadOnly = 2,
  VMReadWrite = 3
};
