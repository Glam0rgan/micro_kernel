#pragma once

typedef struct OsIPCBuffer_ {
    OsMessageInfo tag;
    OsWord msg[OS_MsgMaxLength];
    OsWord userData;
    OsWord caps_or_badges[Os_MsgMaxExtraCaps];
    OsCPtr receiveCNode;
    OsCPtr receiveIndex;
    OsWord receiveDepth;
}OsIPCBuffer __attribute__((__aligned__(sizeof(struct OsIPCBuffer_))));