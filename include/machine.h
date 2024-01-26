#pragma once

#include <plat/machine.h>
#include <machine/registerset.h>
#include <hardware.h>
#include <util.h>

#define PPTR_BASE_OFFSET 0x00

/* When obtaining a physical address from a reference to any object in
 * the physical mapping window, this function must be used. */
static inline PAddr CONST addr_from_pptr(const void* pptr) {
    return (PAddr)pptr - PPTR_BASE_OFFSET;
}

#define paddr_to_pptr(x)   ptr_from_paddr(x)
#define pptr_to_paddr(x)   addr_from_pptr(x)
#define kpptr_to_paddr(x)  addr_from_kpptr(x)
