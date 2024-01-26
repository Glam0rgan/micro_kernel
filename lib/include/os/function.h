#pragma once

#include <os/types.h>
#include <kernel/thread.h>

extern __thread OsIPCBuffer* __osIPCBuffer;

static inline void os_set_ipcBuffer(OsIPCBuffer* ipcBuffer) {
    __osIPCBuffer = ipcBuffer;
    return;
}

static inline OsIPCBuffer* os_get_ipcBuffer(void) {
    return __osIPCBuffer;
}

static inline OsWord os_get_mr(int i) {
    return os_get_ipcBuffer()->msg[i];
}

static inline void os_set_mr(int i, OsWord mr) {
    os_get_ipcBuffer()->msg[i] = mr;
}
