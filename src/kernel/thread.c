#include "types.h"
#include "thread.h"
#include "structures.h"
#include "statedata.h"
#include "util.h"

// Get the Buffer from tcb sender anb receiver
// next invoke do_normal_transfer if ok
// else invoke do_fault_transfer
void do_ipc_transfer(tcb_t* sender, endpoint_t* endpoint,
  u64 badge, bool_t grant, tcb_t* receiver) {
  void* receiveBuffer, * sendBuffer;

  // Get the address of receiver
  receiveBuffer = lookup_ipc_buffer(true, receiver);

  // Guarantee the sender has no fault
  if (likely(sender->tcbFault.os_FaultType) == os_Fault_NullFault) {
    sendBuffer = lookup_ipc_buffer(false, sender);
    do_normal_transfer(sender, sendBuffer, endpoint, badge,
      grant, receiver, receiverBuffer);
  }
  else {
    do_fault_transfer();
  }
}

void do_normal_transfer(tcb_t* sender, u64* sendBuffer, endpoint* endpoint,
  u64 badge, bool_t canGrant, tcb_t* receiver,
  u64* receiveBuffer) {
  u64 msgTransferred;
  os_message_info_t tag;
  exception_t status;

  tag = messageInfo_from_u64(getRegister(sender, msgInfoRegister));

  if (canGrant) { // If sender grant capabilities to receiver

    // Get the extra_caps from sender.
    status = lookup_extra_caps(sender, sendBuffer, tag);

    // If return exception, set excaprefs Null.
    if (unlikely(status != EXCEPTION_NONE)) {
      current_extra_caps.excaprefs[0] = NULL;
    }
  }
  else { // If don't grant , set excaprefs Null.
    current_extra_caps.excaprefs[0] = NULL;
  }

  // Copy sendBuffer to receiveBuffer.
  msgTransferred = copyMRs(sender, sendBuffer, receiver, receiveBuffer,
    tag.length);

  tag = transferCaps(tag, endpoint, receiver, receiveBuffer);

  tag.length = msgTransferred;

  setRegister();
  setRegister();
}

void do_fault_transfer(u64 badge, tcb_t* sender, tcb_t* reveiver,
  u64* reveiverIPCBuffer) {

  u64 msgTransferred;
  messageinfo_t tag;
  exception_t status;
}

void schedule_tcb(tcb_t* tptr) {
  if (tptr == NODE_STATE(ksCurThread) &&
    NODE_STATE(ksSchedulerAction) == SchedulerAction_ResumCurrentThread &&
    !isSchedulable(tptr)) {
    rescheduleRequired();
  }
}

static os_messageinfo_t transfer_caps(os_messageinfo_t info,
  endpoint_t* endpoint, tcb_t* receiver, u64* receiverBuffer) {
  u64 i;
  cte_t* destSlot;

  info.extraCaps = 0;
  info.capsUnwrapped = 0;

  if (likely(!current_extra_caps.excaprefs[0] || !receiveBuffer)) {
    return info;
  }

  destSlot =
}

void set_thread_state(tcb_t* tptr, _thread_state_t ts) {
}

void possible_switch_to(tcb_t* tptr) {
}