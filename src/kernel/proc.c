#include <types.h>
#include <proc.h>
#include <defs.h>
#include <x86.h>
#include <memlayout.h>
#include <mmu.h>
#include <model/statedata.h>
#include <objecttype.h>
#include <cspace.h>
#include <object/endpoint.h>
#include <kernel/thread.h>

extern void trapret(void);

void forkret(void){
  // release lock
  
  cprintf("forkret\n");
  //
  
  //panic("forkret");
}

void alloc_proc(Tcb* tcb){
  
  char* sp;

  tcb->kStack = memblock_alloc_kernel(PGSIZE,PGSIZE);
  
  sp = tcb->kStack + PGSIZE;
  
  // Leave room for trap frame.
  sp -= sizeof *tcb->tf;
  tcb->tf = (Trapframe*)sp;
  
  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= sizeof(uint64_t);
  *(uint64_t*)sp = (uint64_t)trapret;
  
  sp-= sizeof *tcb->context;
  tcb->context = (Context*) sp;
  memset(tcb->context,0,sizeof *tcb->context);
  tcb->context->rip = (uint64_t)forkret; 
}

void test() {
  cprintf("test!!!\n");
}

void exit(){
  sched();
}

void send(OsCPtr dest, OsMessageInfo msgInfo) {
  Cte* cte = lookup_slot(ksCurThread, dest).slot;
  EndpointCap endpointCap = *(EndpointCap*)&cte->cap;
  send_ipc(1, 0, 0, endpointCap.capCanGrant, endpointCap.capCanGrantReply,
    ksCurThread, endpointCap.capEPPtr | 0xFFFF000000000000);

}

void receive(OsCPtr dest, OsMessageInfo* msgInfo) {
  Cte* cte = lookup_slot(ksCurThread, dest).slot;
  receive_ipc(ksCurThread, cte->cap, 1);
}

void print(u64 num) {
  cprintf("num is %l\n", num);
}
