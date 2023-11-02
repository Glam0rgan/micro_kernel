#include <types.h>
#include <ctypes.h>
#include "thread.h"
#include "structures.h"
#include "statedata.h"
#include "util.h"

// Get the Buffer from tcb sender anb receiver
// next invoke do_normal_transfer if ok
// else invoke do_fault_transfer
void do_ipc_transfer(Tcb* sender, Endpoint* endpoint,
  u64 badge, bool grant, Tcb* receiver) {
  void* receiveBuffer, * sendBuffer;

  // Get the address of receiver
  receiveBuffer = lookup_ipc_buffer(true, receiver);

  // Guarantee the sender has no fault
  if(likely(sender->tcbFault.os_FaultType) == os_Fault_NullFault) {
    sendBuffer = lookup_ipc_buffer(false, sender);
    do_normal_transfer(sender, sendBuffer, endpoint, badge,
      grant, receiver, receiveBuffer);
  } else {
    do_fault_transfer();
  }
}

void do_normal_transfer(Tcb* sender, u64* sendBuffer, Endpoint* endpoint,
  u64 badge, bool canGrant, Tcb* receiver,
  u64* receiveBuffer) {
  u64 msgTransferred;
  OsMessageInfo tag;
  Exception status;

  tag = messageinfo_from_u64(getRegister(sender, msgInfoRegister));

  if(canGrant) { // If sender grant capabilities to receiver

    // Get the extra_caps from sender.
    status = lookup_extra_caps(sender, sendBuffer, tag);

    // If return exception, set excaprefs Null.
    if(unlikely(status != EXCEPTION_NONE)) {
      current_extra_caps.excaprefs[0] = NULL;
    }
  } else { // If don't grant , set excaprefs Null.
    current_extra_caps.excaprefs[0] = NULL;
  }

  // Copy sendBuffer to receiveBuffer.
  msgTransferred = copyMRs(sender, sendBuffer, receiver, receiveBuffer,
    tag.length);

  tag = transfer_caps(tag, endpoint, receiver, receiveBuffer);

  tag.length = msgTransferred;

  setRegister(receiver, msgInfoRegister, u64_from_messageinfo(msgInfo));
  setRegister(reveiver, badgeRegister, badge);
}

void do_fault_transfer(u64 badge, Tcb* sender, Tcb* reveiver,
  u64* reveiverIPCBuffer) {

  u64 msgTransferred;
  OsMessageInfo tag;
  Exception status;
}

void schedule_tcb(Tcb* tptr) {
  if(tptr == NODE_STATE(ksCurThread) &&
    NODE_STATE(ksSchedulerAction) == SchedulerAction_ResumCurrentThread &&
    !isSchedulable(tptr)) {
    rescheduleRequired();
  }
}

// Like getReceiveSlots, this is specialised for single-cap transfer.
static OsMessageInfo transfer_caps(OsMessageInfo info,
  Endpoint* endpoint, Tcb* receiver, u64* receiverBuffer) {
  u64 i;
  Cte* destSlot;

  info.extraCaps = 0;
  info.capsUnwrapped = 0;

  if(likely(!current_extra_caps.excaprefs[0] || !receiveBuffer)) {
    return info;
  }

  destSlot =
}

void possible_switch_to(Tcb* tptr) {

}
