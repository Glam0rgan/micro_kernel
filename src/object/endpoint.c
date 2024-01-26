#include <types.h>
#include <string.h>
#include <os/constants.h>
#include <kernel/thread.h>
#include <kernel/vspace.h>
#include <machine/registerset.h>
#include <model/statedata.h>
#include <object/notification.h>
#include <object/cnode.h>
#include <object/tcb.h>
#include <object/endpoint.h>


// Send ipc
void send_ipc(bool blocking, bool doCall, u64 badge,
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
      (&thread->tcbState)->blockingIPCIscall = doCall;

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

    dest->tcbState.tsType = ThreadState_Running;
    possible_switch_to(dest);

    if(doCall) {
      if(canGrant || canGrantReply) {
        setup_caller_cap(thread, dest, replyCanGrant);
      } else {
        thread->tcbState.tsType = ThreadState_Running;
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
  
  TcbQueue queue;
  
  epptr = EP_PTR(endpointCap.capEPPtr);

  ntfnPtr = thread->tcbBoundNotification;
  if(ntfnPtr && ntfnPtr->state == NtfnState_Active) {
    complete_signal(ntfnPtr, thread);
  } else {
    switch(epptr->state) {
    case EPState_Idle:
    case EPState_Recv:
      
      if(isBlocking) {
        // Set thread state to BlockedOnReveive

      }
    case EPState_Send:
      Tcb* sender;
      u64 badge;
      bool canGrant;
      bool canGrantReply;
      bool doCall;

      queue = epptr_get_queue(epptr);
      sender = queue.head;

      // panic(sender);

      queue = tcb_ep_dequeue(sender, queue);
      epptr_set_queue(epptr, queue);

      if(!queue.head) {
        epptr->state = EPState_Idle;
      }

      // Get sender IPC details
      badge = (&sender->tcbState)->blockingIPCBadge;
      canGrant = (&sender->tcbState)->blockingIPCCanGrant;
      canGrantReply = (&sender->tcbState)->blockingIPCCangrantreply;

      do_ipc_transfer(sender, epptr, badge, canGrant, thread);

      doCall = (&sender->tcbState)->blockingIPCIscall;

      if(doCall) {
        if(canGrant || canGrantReply) {
          setup_caller_cap(sender, thread, endpointCap.capCanGrant);
        } else {
          sender->tcbState.tsType = ThreadState_Inactive;
        }
      } else {
      	sender->tcbState.tsType = ThreadState_Running;
        possible_switch_to(sender);
      }
      break;
    }
  }
}

void cancel_ipc(Tcb* tptr) {
  ThreadState* state = &tptr->tcbState;

  switch(state->tsType) {
  case ThreadState_BlockedOnSend:
  case ThreadState_BlockedOnReceive: {
    // blockedIPCCancel state
    Endpoint* epptr;
    TcbQueue queue;

    epptr = EP_PTR(state->blockingObject);

    // Error "blockedIPCCancel: endpoint must not be idle" 
    // panic();

    // Dequeue TCB.
    queue = epptr_get_queue(epptr);
    queue = tcb_ep_dequeue(tptr, queue);
    epptr_set_queue(epptr, queue);

    if(!queue.head) {
      epptr->state = EPState_Idle;
    }

    tptr->tcbState.tsType = ThreadState_Inactive;
    break;
  }
  case ThreadState_BlockedOnNotification:
  case ThreadState_BlockedOnReply: {

  }
  }
}
