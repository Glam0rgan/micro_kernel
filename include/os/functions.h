#include <structures.h>

extern __thread IpcBuffer* __OsIpcBuffer;

IpcBuffer* Osget_ipc_buffer(void) {
  return __OsIpcBuffer;
}

u64 Osget_mr(int i) {
  return Osget_ipc_buffer()->msg[i];
}