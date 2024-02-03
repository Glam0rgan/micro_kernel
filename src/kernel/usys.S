#include <kernel/syscall.h>
#include <kernel/traps.h>

.globl send;
send:
  movl $2, %eax;
  int $T_SYSCALL;
  ret
.globl receive;
receive:
  movl $3, %eax;
  int $T_SYSCALL;
  ret
.globl test;
test:
  movl $1, %eax;
  int $T_SYSCALL;
  ret
.globl print;
print:
  movl $4, %eax;
  int $T_SYSCALL;
  ret
.globl exit;
exit:
  movl $5, %eax;
  int $T_SYSCALL;
  ret



  

