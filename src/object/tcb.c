#include <types.h>
#include <failures.h>
#include <structures.h>
#include <util.h>
#include <tcb.h>
#include <arch/x86/arch/64/mode/machine/registerset.h>

ExtraCaps currentExtraCaps;

Exception lookup_extra_caps(Tcb* thread, u64* bufferPtr, OsMessageInfo info) {

  LookupSlotRawRet luRet;
  Cptr cptr;
  u64 i, length;

  // If the buffer pointer is Null, set the extra_caps
  // Null and return.
  if(!bufferPtr) {
    currentExtraCaps.excaprefs[0] = NULL;
    return EXCEPTION_NONE;
  }

  // The length is 2 bits.
  length = info.extraCaps;

  for(i = 0; i < length; i++) {
    // Get ptr and then look-up the slot
    cptr = get_extra_cptr(bufferPtr, i);
    luRet = lookup_slot(thread, cptr);

    // Check the status
    if(luRet.status != EXCEPTION_NONE) {
      currentFault = os_fault_capfault_new(cptr, false);
      return luRet.status;
    }


    // Add slots to excaprefs
    currentExtraCaps.excaprefs[i] = luRet.slot;
  }

  if(i < os_MsgMaxExtraCaps) {
    currentExtraCaps.excaprefs[i] = NULL;
  }

  return EXCEPTION_NONE;
}

// Copy IPC MRs from one thread to another
u64 copyMRs(Tcb* sender, u64* sendBuf, Tcb* receiver,
  u64* recvBuf, u64 n) {
  u64 i;

  // Copy inline words (in registers)
  for(i = 0; i < n && i < n_msgRegisters; i++) {
    setRegister(receiver, msgRegisters[i],
      getRegister(sender, msgRegisters[i]));
  }

  // Don't have recvBuf or sendBuf, should return.  
  if(!recvBuf || !sendBuf) {
    return i;
  }

  // Copy out-of-line words (in memory)
  for(; i < n; i++) {
    recvBuf[i + 1] = sendBuf[i + 1];
  }

  return i;
}

void setup_caller_cap(Tcb* sender, Tcb* receiver, bool canGrant) {

}

// Add tcb to an endpoint queue
TcbQueue tcb_ep_append(Tcb* tcb, TcbQueue queue) {
  if(!queue.head)
    queue.head = tcb;
  else
    queue.end->tcbEPNext = tcb;
  tcb->tcbEPPrev = queue.end;
  tcb->tcbEPNext = NULL;
  queue.end = tcb;
}


// Remove tcb from an endpoint queue
TcbQueue tcb_ep_dequeue(Tcb* tcb, TcbQueue queue) {
  if(tcb->tcbEPPrev) {
    tcb->tcbEPPrev->tcbEPNext = tcb->tcbEPNext;
  } else {
    queue.head = tcb->tcbEPNext;
  }

  if(tcb->tcbEPNext) {
    tcb->tcbEPNext->tcbEPPrev = tcb->tcbEPPrev;
  } else {
    queue.end = tcb->tcbEPPrev;
  }

  return queue;
}

// Get the capability pointer position i in ipc_buffer
Cptr PURE get_extra_cptr(u64* bufferPtr, u64 i) {
  // The buffer structer
  // tag(8 bytes) msg(8* os_MsgMaxLength bytes ) user_data(8 byrtes)
  // caps_or_badges
  return (Cptr)bufferPtr[os_MsgMaxLength + 2 + i];
}

void set_extra_badge(u64* bufferPtr, u64 badge, u64 i) {
  bufferPtr[os_MsgMaxLength + 2 + i] = badge;
}

// Add TCB to the head of a scheduler queue.
void tcb_sched_enqueue(Tcb* tcb) {
  if(!tcb->tcbState.tcbQueued) {
    TcbQueue queue;
    Dom dom;
    Prio prio;
    u64 idx;

    dom = tcb->tcbDomain;
    prio = tcb->tcbPriority;
    idx = ready_queues_index(dom, prio);
    queue = NODE_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

    if(!queue.head) { // Empty List
      queue.end = tcb;
      // fix
      // add_to_bitmap();
    } else {
      queue.head->tcbSchedPrev = tcb;
    }
    tcb->tcbSchedPrev = NULL;
    tcb->tcbSchedNext = queue.head;
    queue.head = tcb;

    NODE_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = queue;

    &tcb->tcbState.tcbQueued = true;
  }
}

// Add TCB to the end of a scheduler queue.
void tcb_sched_append(Tcb* tcb) {
  if(!tcb->tcbState.tcbQueued) {
    TcbQueue queue;
    Dom dom;
    Prio prio;
    u64 idx;

    dom = tcb->tcbDomain;
    prio = tcb->tcbPriority;
    idx = ready_queues_index(dom, prio);
    queue = NODE_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

    if(!queue.head) { // Empty List
      queue.head = tcb;
      // fix
      // add_to_bitmap();
    } else {
      queue.end->tcbSchedPrev = tcb;
    }
    tcb->tcbSchedPrev = queue.end;
    tcb->tcbSchedNext = NULL;
    queue.end = tcb;

    NODE_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = queue;

    &tcb->tcbState.tcbQueued = true;
  }
}

// Remove TCB from a scheduler queue.
void tcb_sched_dequeue(Tcb* tcb) {
  if(tcb->tcbState.tcbQueued) {
    TcbQueue queue;
    Dom dom;
    Prio prio;
    u64 idx;

    dom = tcb->tcbDomain;
    prio = tcb->tcbPriority;
    idx = ready_queues_index(dom, prio);
    queue = NODE_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

    if(tcb->tcbSchedPrev) {
      tcb->tcbSchedPrev->tcbSchedNext = tcb->tcbSchedNext;
    } else {
      queue.head = tcb->tcbSchedNext;
      if(likely(!tcb->tcbSchedNext)) {
        // fix
        // remove_from_bitmap
      }
    }


    if(tcb->tcbSchedNext) {
      tcb->tcbSchedNext->tcbSchedPrev = tcb->tcbSchedPrev;
    } else {
      queue.end = tcb->tcbSchedPrev;
      if(likely(!tcb->tcbSchedPrev)) {
        // fix
        // remove_from_bitmap
      }
    }

    NODE_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = queue;

    &tcb->tcbState.tcbQueued = false;
  }
}