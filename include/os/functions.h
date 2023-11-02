#include "structures.h"

extern __thread IpcBuffer* __OsIpcBuffer;

IpcBuffer* os_get_ipc_buffer(void) {
  return __OsIpcBuffer;
}

u64 os_get_mr(int i) {
  return os_get_ipc_buffer()->msg[i];
}