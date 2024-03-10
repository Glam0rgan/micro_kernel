#pragma once

#include <util.h>
#include <types.h>
#include <object/tcb.h>
#include <object/structures.h>

// Use endpoint pointer get the thread queue.
static inline TcbQueue PURE epptr_get_queue(Endpoint* epptr) {
  TcbQueue queue;

  queue.head = (Tcb*)epptr->epQueueHead;
  queue.end = (Tcb*)epptr->epQueueTail;

  return queue;
}


// Set the thread queue.
static inline void epptr_set_queue(Endpoint* epptr, TcbQueue queue) {
  epptr->epQueueHead = (u64)queue.head;
  epptr->epQueueTail = (u64)queue.end;
}

void send_ipc(bool blocking, bool doCall, u64 badge,
  bool canGrant, bool canGrantReply, Tcb* thread,
  Endpoint* epptr);

void receive_ipc(Tcb* thread, Cap cap, bool isBlocking);

void cancel_ipc(Tcb* tcb);
