#include <syscall.h>
#include <traps.h>

#define SYSCALL(name) \
  .globl name; \
  name: \
    movl $SYS_ ## name, %eax; \
    int $T_SYSCALL; \
    ret

SYSCALL(send)
SYSCALL(receive)
SYSCALL(test)
SYSCALL(print)