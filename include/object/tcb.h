#pragma once

#include <types.h>
#include <api/failures.h>
#include <object/structures.h>

#include <machine/registerset.h>
#include <object/cnode.h>

void tcb_sched_enqueue(Tcb* tcb);
void tcb_sched_append(Tcb* tcb);
void tcb_sched_dequeue(Tcb* tcb);

#define SCHED_ENQUEUE(_t)           tcb_sched_enqueue(_t)


typedef struct _TcbQueue {
  Tcb* head;
  Tcb* end;
} TcbQueue;

void setup_caller_cap(Tcb* sender, Tcb* receiver, bool canGrant);
TcbQueue tch_ep_append(Tcb* tcb, TcbQueue queue);
TcbQueue tcb_ep_dequeue(Tcb* tcb, TcbQueue queue);
u64 copyMRs(Tcb* sender, u64* sendBuf, Tcb* receiver,
  u64* recvBuf, u64 n);

CPtr PURE get_extra_cptr(u64* bufferPtr, u64 i);

void set_extra_badge(u64* bufferPtr, u64 badge, u64 i);

Exception lookup_extra_caps(Tcb* thread, u64* bufferPtr, OsMessageInfo info);
