#pragma once

#include <types.h>
#include <object/structures.h>

// Use endpoint pointer get the thread queue.
static inline TcbQueue PURE epptr_get_queue(Endpoint* epptr) {
  TcbQueue queue;

  queue.head = (Tcb*)epptr.epQueue_head;
  queue.end = (Tcb*)epptr.epQueue_tail;

  return queue;
}


// Set the thread queue.
static inline void epptr_set_queue(Endpoint* epptr, TcbQueue queue) {
  epptr->epQueue_head = (u64)queue.head;
  epptr->epQueue_head = (u64)queue.end;
}

void send_ipc(bool blocking, bool doCall, u64 badge,
  bool canGrant, bool canGrantReply, Tcb* thread,
  Endpoint* epptr);

void cancel_ipc(Tcb* tptr);