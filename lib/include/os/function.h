#pragma once

#include <os/types.h>

extern __thread OsIPCBuffer* __osIPCBuffer;

static inline void Osset_ipcBuffer(OsIPCBuffer* ipcBuffer) {
    __osIPCBuffer = ipcBuffer;
    return;
}

static inline OsIPCBuffer* Osget_ipcBuffer(void) {
    return __osIPCBuffer;
}

static inline OsWord OsgetMR(int i) {
    return Osget_ipcBuffer()->msg[i];
}

static inline void OssetMr(int i, OsWord mr) {
    Osget_ipcBuffer()->msg[i] = mr;
}