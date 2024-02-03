#include <types.h>
#include <syscall.h>
#include <proc.h>
#include <defs.h>
#include <model/statedata.h>

// arguments passed in registers on x64
static uint64_t fetch_arg(int n) {
  switch(n) {
  case 0: return ksCurThread->tf->rdi;
  case 1: return ksCurThread->tf->rsi;
  case 2: return ksCurThread->tf->rdx;
  case 3: return ksCurThread->tf->rcx;
  case 4: return ksCurThread->tf->r8;
  case 5: return ksCurThread->tf->r9;
  }
}

int arg_int(int n, int* ip) {
  *ip = fetch_arg(n);
  return 0;
}

int arg_uint64(int n, uint64_t* ip) {
  *ip = fetch_arg(n);
  return 0;
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size n bytes.  Check that the pointer
// lies within the process address space.
int arg_ptr(int n, char** pp, int size) {
  uint64_t i;

  if(arg_uint64(n, &i) < 0)
    return -1;
  *pp = (char*)i;
  return 0;
}

extern int sys_test(void);
extern int sys_send(void);
extern int sys_receive(void);
extern int sys_exit(void);
extern int sys_print(void);

static int (*syscalls[])(void) = {
  [SYS_test] sys_test,
  [SYS_send]    sys_send,
  [SYS_receive] sys_receive,
  [SYS_exit]    sys_exit,
  [SYS_print]   sys_print
};

void syscall(void) {
  int num;

  num = ksCurThread->tf->rax;
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    ksCurThread->tf->rax = syscalls[num]();
  } else {
    panic("syscall_error");
  }
}
