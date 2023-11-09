#include <types.h>
#include <string.h>
#include <os/constants.h>
#include <kernel/thread.h>
#include <kernel/vspace.h>
#include <machine/registerset.h>
#include <model/statedata.h>
#include <object/notification.h>
#include <object/cnode.h>
#include <object/endpoint.h>
#include <object/tcb.h>

// Send ipc
void send_ipc(bool blocking, bool do_call, u64 badge,
  bool canGrant, bool canGrantReply,
  Tcb* thread, Endpoint* epptr) {
  switch(epptr->state) {
  case EPState_Idle:
  case EPState_Send:
    if(blocking) {
      TcbQueue queue;

      // Set thread state to BlockedOnSend
      (&thread->tcbState)->tsType = ThreadState_BlockedOnSend;
      (&thread->tcbState)->blockingObject = (u64)epptr;
      (&thread->tcbState)->blockingIPCBadge = badge;
      (&thread->tcbState)->blockingIPCCanGrant = canGrant;
      (&thread->tcbState)->blockingIPCCangrantreply = canGrantReply;
      (&thread->tcbState)->blockingIPCIscall = do_call;

      schedule_tcb(thread);

      // Place calling thread in endpoint queue
      queue = epptr_get_queue(epptr);
      queue = tcb_ep_append(thread, queue);
      epptr->state = EPState_Send;
      epptr_set_queue(epptr, queue);
    }
    break;

  case EPState_Recv:
    TcbQueue queue;
    Tcb* dest;

    // Get the head of the endpoint queue
    queue = epptr_get_queue(epptr);
    dest = queue.head;

    //if(dest == NULL)
    //  panic();

    // Dequeue the first TCB
    queue = tcb_ep_dequeue(dest, queue);
    epptr_set_queue(epptr, queue);

    if(!queue.head) {
      epptr->state = EPState_Idle;
    }

    // Do the transfer
    do_ipc_transfer(thread, epptr, badge, canGrant, dest);

    bool replyCanGrant = (&dest->tcbState)->blockingIPCCanGrant;

    dest.threadState = ThreadState_Running;
    possible_switch_to(dest);

    if(do_call) {
      if(canGrant || canGrantReply) {
        setup_caller_cap(thread, dest, replyCanGrant);
      } else {
        thread->threadState = ThreadState_Inactive;
      }
    }
    break;
  }
}

void receive_ipc(Tcb* thread, Cap cap, bool isBlocking) {
  Endpoint* epptr;
  Notification* ntfnPtr;

  //if(unlikely(cap.capType != cap_endpoint_cap))
    //panic();

  EndpointCap endpointCap = *(EndpointCap*)(&cap);

  epptr = EP_PTR(endpointCap.capEPPtr);

  ntfnPtr = thread->tcbBoundNotification;
  if(ntfnPtr && ntfnPtr->state == NtfnState_Active) {
    complete_signal(ntfnPtr, thread);
  } else {
    switch(epptr->state) {
    case EPState_Idle:
    case EPState_Recv:
      TcbQueue queue;
      if(isBlocking) {
        // Set thread state to BlockedOnReveive

      }
    case EPState_Send:
      TcbQueue;
      Tcb* sender;
      u64 badge;
      bool canGrant;
      bool canGrantReply;
      bool do_call;

      queue = epptr_get_queue(epptr);
      sender = queue.head;

      // panic(sender);

      queue = tcb_ep_dequeue(sender, queue);
      epptr_set_queue(epptr, queue);

      if(!queue.head) {
        epptr->state = EPState_Idle;
      }

      // Get sender IPC details
      badge = (&sender->tcb_state).blockingIPCBadge;
      canGrant = (&sender->tcb_state).blockingIPCCanGrant;
      canGrantReply = (&sender->tcb_state).blockingIPCCanGrantReply;

      do_ipc_transfer(sender, epptr, badge, canGrant, thread);

      do_call = (&sender->tcb_state).blockingIPCIsCall;

      if(do_call) {
        if(canGrant || canGrantReply) {
          setup_caller_cap(sender, thread, endpointCap.canGrant);
        } else {
          sender.threadState = ThreadState_Inactive;
        }
      } else {
        sender.threadState = ThreadState_Running;
        possible_switch_to(sender);
      }
      break;
    }
  }
}