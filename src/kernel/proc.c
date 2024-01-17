#include <proc.h>
#include <defs.h>
#include <x86.h>
#include <types.h>
#include <memlayout.h>
#include <mmu.h>
#include <model/statedata.h>
#include <objecttype.h>
#include <cspace.h>

void test() {
  panic("test!!!");
}

void send(OsCPtr dest, OsMessageInfo msgInfo) {
  Cte* cte = lookup_slot(ksCurThread, 0).slot;
  EndpointCap endpointCap = *(EndpointCap*)&cte->cap;
  send_ipc(1, 0, 0, endpointCap.capCanGrant, endpointCap.capCanGrantReply,
    ksCurThread, endpointCap.capEPPtr);

}

void reply(OsMessageInfo msgInfo) {
  Cte* cte = lookup_slot(ksCurThread, 0).slot;
  receive_ipc(ksCurThread, cte->cap, 1);
}