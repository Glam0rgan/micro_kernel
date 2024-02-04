#include <types.h>
#include <x86.h>
#include <defs.h>
#include <param.h>
#include <memlayout.h>
#include <mmu.h>
#include <proc.h>
#include <kernel/thread.h>
#include <object/structures.h>

extern Tcb* ksCurThread;

int sys_test(void) {
    test();
    return 0;
}

int sys_exit(void){
    exit();
    return 0;
}

int sys_send(void) {
    OsCPtr ptr;
    OsMessageInfo msg;

    uint64_t temp;

    arg_uint64(0, &temp);
    ptr = (OsCPtr)temp;

    msg = *(OsMessageInfo*)(&ksCurThread->tf->rsi);

    send(ptr, msg);
    return 0;
}

int sys_print(void){
   uint64_t num;
   arg_uint64(0, &num);
   
   print(num);
}

int sys_receive(void) {
    OsCPtr ptr;
    OsMessageInfo* msg;

    uint64_t temp;

    arg_uint64(0, &temp);
    ptr = (OsCPtr)temp;

    arg_uint64(1, &temp);
    msg = (OsMessageInfo*)&temp;

    receive(ptr, msg);
    return 0;
}
