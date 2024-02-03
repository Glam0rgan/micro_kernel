#include <types.h>
#include <x86.h>
#include <traps.h>
#include <proc.h>
#include <model/statedata.h>

void trap(Trapframe* tf) {
  if(tf->trapno == T_SYSCALL) {
    ksCurThread->tf = tf;
    syscall();
    return;
  }
}
