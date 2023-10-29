#include "types.h"
#include "structures.h"
#include "endpoint.h"
#include "cnode.h"
#include "tcb.h"
#include "thread.h"

void send_ipc(bool_t blocking, bool_t do_call, u64 badge,
              bool_t canGrant, bool_t canGrantReply, 
              tcb_t* thread, endpoint_t* epptr){
  switch(ep_ptr_get_state(epptr)){
    case EPState_Idle:
    case EPState_Send:
      if(blocking){
        tcb_queue_t queue;

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
      tcb_queue_t queue;
      tcb_t* dest;

      // Get the head of the endpoint queue
      queue = epptr_get_queue(epptr);
      dest = queue.head;
      
      //if(dest == NULL)
      //  panic();
      
      // Dequeue the first TCB
      queue = tcb_ep_dequeue(dest, queue);
      epptr_set_queue(epptr, queue);

      if(!queue.head){
        epptr->state = EPState_Idle;
      }
      
      // Do the transfer
      do_ipc_transfer(thread, epptr, badge, canGrant, dest);

      bool_t replyCanGrant = (&dest->tcbState)->blockingIPCCanGrant;

      set_thread_state(dest, ThreadState_Running);
      possible_switch_to(dest);
      
      if(do_call){
        if(canGrant || canGrantReply){
          setup_caller_cap(thread, dest, replyCanGrant);
        }
        else{
          set_thread_state(thread, ThreadState_Inactive);
        }
      }
      break;
  }
}

void receive_ipc(tcb_t* thread, cap_t cap, bool_t isBlocking){
  endpoint_t* epptr;
  notification_t* ntfnPtr;
  
  //if(unlikely(cap.capType != cap_endpoint_cap))
    //panic();
  
  endpoint_cap_t* endpoint_cap_ptr = (endpoint_cap_t*)(&cap);
  endpoint_cap_t endpoint_cap = *endpoint_cap_ptr;

  epptr = EP_PTR(endpoint_cap.capEPPtr);

  ntfnPtr = thread->tcbBoundNotification;
  if(ntfnPtr && ntfnPtr->state == NtfnState_Active){

  }
  else{
    switch(epptr->state){
      case EPState_Idle:
      case EPState_Recv:
        tcb_queue_t queue;
        if(isBlocking){
          // Set thread state to BlockedOnReveive

        }
      case EPState_Send:
        tcb_queue_t;
        tcb_t* sender;
        u64 badge;
        bool_t canGrant;
        bool_t canGrantReply;
        bool_t do_call;

        queue = epptr_get_queue(epptr);
        sender = queue.head;

        // Haskell
        // assert(sender);

        queue = tcb_ep_dequeue(sender, queue);
        epptr_set_queue(epptr, queue);

        if(!queue.head){
          epptr->state = EPState_Idle;
        }
        
        // Get sender IPC details
        badge = thread_state_ptr_get_blockingIPCBadge(&sender->tcb_state);
        canGrant = thread_state_ptr_get_blockingIPCCanGrant(&sender->tcb_state);
        canGrantReply = thread_state_ptr_get_blockingIPCCanGrantReply(&sender->tcb_state);

        do_ipc_transfer(sender, epptr, badge, canGrant, thread);

        do_call = thread_state_ptr_get_blockingIPCIsCall(&sender->tcb_state);

        if(do_call){
          if(canGrant || canGrantReply){

          }
          else{

          }
        }
        else{

        }
        break;
    }
  }
}