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

void test() {
  panic("test!!!");
}

void send(OsCPtr dest, OsMessageInfo msgInfo) {
  Cte* cte = lookup_slot(ksCurThread, dest).slot;
  EndpointCap endpointCap = *(EndpointCap*)&cte->cap;
  send_ipc(1, 0, 0, endpointCap.capCanGrant, endpointCap.capCanGrantReply,
    ksCurThread, endpointCap.capEPPtr);

}

void receive(OsCPtr dest, OsMessageInfo* msgInfo) {
  Cte* cte = lookup_slot(ksCurThread, dest).slot;
  receive_ipc(ksCurThread, cte->cap, 1);
}

void print(char* fmt, ...) {

}
