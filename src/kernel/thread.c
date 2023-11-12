#include <api/types.h>
#include <object.h>
#include <api/faults.h>
#include <util.h>
#include <kernel/cspace.h>
#include <kernel/thread.h>
#include <kernel/vspace.h>
#include <model/statedata.h>
#include <arch/machine.h>
#include <arch/kernel/thread.h>
#include <machine/registerset.h>

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
    do_fault_transfer(badge, sender, receiver, receiveBuffer);
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

  setRegister(receiver, msgInfoRegister, u64_from_messageinfo(tag));
  setRegister(receiver, badgeRegister, badge);
}

void do_fault_transfer(u64 badge, Tcb* sender, Tcb* reveiver,
  u64* reveiverIPCBuffer) {

}

// Resume the current thread and invoke reschedule_required
// to add ksSchedulerAction to the scheduler queue
void schedule_tcb(Tcb* tptr) {
  if(tptr == NODE_STATE(ksCurThread) &&
    NODE_STATE(ksSchedulerAction) == SchedulerAction_ResumCurrentThread &&
    !isSchedulable(tptr)) {
    reschedule_required();
  }
}

// Like getReceiveSlots, this is specialised for single-cap transfer.
static OsMessageInfo transfer_caps(OsMessageInfo info,
  Endpoint* endpoint, Tcb* receiver, u64* receiveBuffer) {
  u64 i;
  Cte* destSlot;

  info.extraCaps = 0;
  info.capsUnwrapped = 0;

  // If extra caps is empty or don't have receive buffer.
  if(likely(!current_extra_caps.excaprefs[0] || !receiveBuffer)) {
    return info;
  }

  destSlot = get_receive_slots(receiver, receiveBuffer);

  for(i = 0; i < os_MsgMaxExtraCaps && currentExtraCaps.excaprefs[i] != NULL; i++) {
    Cte* slot = currentExtraCaps.excaprefs[i];
    Cap cap = slot->cap;
    EndpointCap endpointCap = *(EndpointCap*)(&cap);

    if(endpointCap.capType == cap_endpoint_cap &&
      EP_PTR(endpointCap.capEPPtr) == endpoint) {
      // If this is a cap to the endpoint on which the message was sent,
      // only transfer the badge, not the cap.
      set_extra_badge(receiveBuffer, cap.capEPBadge, i);

      info.capsUnwrapped = info.capsUnwrapped | (1 << i);
    } else {
      DeriveCapRet dcRet;

      if(!destSlot) {
        break;
      }

      if(dcRet.status != EXCEPTION_NONE) {
        break;
      }
      if(dcRet.cap.capType != cap_null_cap) {
        break;
      }

      cte_inssert(dcRet.cap, slot, destSlot);

      destSlot = NULL;
    }
  }

  info.extraCaps = i;
  return info;
}

void do_nbRecvFailed_transfer(Tcb* thread) {
  /* Set the badge register to 0 to indicate there was no message */
  set_register(thread, badgeRegister, 0);
}

void switch_to_thread(Tcb* thread) {
  arch_switch_to_thread(thread);
  tcb_sched_dequeue(thread);
  NODE_STATE(ksCurThread) = thread;
}

static void schedule_choose_newThread(void) {
  choose_thread();
}

void schedule(void) {
  if(ksSchedulerAction != SchedulerAction_ResumeCurrentThread) {
    bool wasRunnable;
    if(is_runnable(ksCurThread)) {
      wasRunnable = true;
      tcb_sched_enqueue(ksCurThread);
    } else {
      wasRunnable = false;
    }

    if(ksSchedulerAction == SchedulerAction_ChooseNewThread) {
      schedule_choose_newThread();
    } else {
      schedule_choose_newThread();
    }
  }
  ksSchedulerAction = SchedulerAction_ResumeCurrentThread;
}

void choose_thread(void) {
  thread = ksReadyQueue.head;
  switch_to_thread(thread);
}

/*
Note that this thread will possibly continue at the end of this kernel
entry. Do not queue it yet, since a queue+unqueue operation is wasteful
if it will be picked. Instead, it waits in the 'ksSchedulerAction' site
on which the scheduler will take action.
*/
void possible_switch_to(Tcb* target) {
  if(ksCurDomain != target->tcbDomain
    SMP_COND_STATEMENT(|| target->tcbAffinity != getCurrentCPUIndex())) {
    SCHED_ENQUEUE(target);
  } else if(NODE_STATE(ksSchedulerAction) != SchedulerAction_resumeCurrentThread) {
    // Too many threads want special treatment, use regular queues.
    reschedule_required();
    SCHED_ENQUEUE(target);
  } else {
    NODE_STATE(ksSchedulerAction) = target;
  }
}

void reschedule_required(void) {
  if(NODE_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread
    && NODE_STATE(ksSchedulerAction) != SchedulerAction_ChooseNewThread) {
    SCHED_ENQUEUE(NODE_STATE(ksSchedulerAction));
  }
  NODE_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
}

void set_threadState(Tcb* tptr, _ThreadState ts) {
  tptr->tcbState.tsType = ts;
  schedule_tcb(tptr);
}
