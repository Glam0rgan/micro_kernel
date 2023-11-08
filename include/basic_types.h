#pragma once

#include <ctypes.h>
#include <arch/types.h>

/* for lib headers that the kernel shares */
typedef uint8_t os_u8;
typedef uint16_t os_u16;
typedef uint32_t os_u32;
typedef CPtr OsCPtr;
typedef NodeId OsNodeId;
typedef PAddr OsPAddr;
typedef Dom OsDomain;