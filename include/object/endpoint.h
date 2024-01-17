#pragma once

#include <types.h>
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

void cancel_ipc(Tcb* tptr);