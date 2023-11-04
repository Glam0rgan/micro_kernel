#include "structures.h"
#include "util.h"
#include "tcb.h"

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
u64 copyMRs(tcb_t* sender, u64* sendBuf, tcb_t* receiver,
  u64* recvBuf, u64 n) {
  u64 i;

  // Copy inline words (in registers)
  for(i = 0; i < n && i < n_msgRegisters; i++) {
    setRegister();
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

void setup_caller_cap(tcb_t* sender, tcb_t* receiver, bool_t canGrant) {

}

// Add tcb to an endpoint queue
tcb_queue_t tch_ep_append(tcb_t* tcb, tcb_queue_t queue) {
  if(!queue.head)
    queue.head = tcb;
  else
    queue.end->tcbEPNext = tcb;
  tcb->tcbEPPrev = queue.end;
  tcb->tcbEPNext = NULL;
  queue.end = tcb;
}


// Remove tcb from an endpoint queue
tcb_queue_t tcb_ep_dequeue(tcb_t* tcb, tcb_queue_t queue) {
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
cptr_t PURE get_extra_cptr(u64* bufferPtr, u64 i) {
  // The buffer structer
  // tag(8 bytes) msg(8* os_MsgMaxLength bytes ) user_data(8 byrtes)
  // caps_or_badges
  return (cptr_t)bufferPtr[os_MsgMaxLength + 2 + i];
}

void set_extra_badge(u64* bufferPtr, u64 badge, u64 i) {
  bufferPtr[os_MsgMaxLength + 2 + i] = badge;
}