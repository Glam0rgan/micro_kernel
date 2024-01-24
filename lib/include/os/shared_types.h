#pragma once
#include <mode_include/64/os/shared_types.h>


typedef struct OsIPCBuffer_ {
    OsMessageInfo tag;
    OsWord msg[Os_MsgMaxLength];
    OsWord userData;
    OsWord caps_or_badges[Os_MsgMaxExtraCaps];
    OsCPtr receiveCNode;
    OsCPtr receiveIndex;
    OsWord receiveDepth;
}OsIPCBuffer __attribute__((__aligned__(sizeof(struct OsIPCBuffer_))));
