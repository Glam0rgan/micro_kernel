#include "structures.h"

extern __thread ipc_buffer* __os_ipc_buffer;

ipc_buffer* os_get_ipc_buffer(void){
  return __os_ipc_buffer;
}

u64 os_get_mr(int i){
  return os_get_ipc_buffer()->msg[i];
}