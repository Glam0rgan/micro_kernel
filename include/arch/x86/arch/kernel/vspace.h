#pragma once

#include <config.h>
#include <types.h>
#include <api/failures.h>
#include <object/structures.h>

typedef struct _FindVSpaceForASIDRet {
    Exception status;
    VSpaceRoot* vspaceRoot;
}FindVSpaceForASIDRet;

u64* PURE lookup_ipcbuffer(bool isReceiver, Tcb* thread);