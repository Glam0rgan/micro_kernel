#include "types.h"
#include "endpoint.h"
#include "cnode.h"
#include "tcb.h"
#include "structures.h"

#define cap_endpoint_cap_get_capEPPtr(cap) (cap.low&MASK(48))

void send_ipc(bool_t blocking, bool_t do_call, u64 badge,
              bool_t canGrant, bool_t canGrantReply, 
              tcb_t* thread, endpoint_t* epptr){
  switch(ep_ptr_get_state(epptr)){
    case EPState_Idle:
    case EPState_Send:
      if(blocking){
        tcb_queue_t queue;

        // Set thread state to BlockedOnSend

        // Place calling thread in endpoint queue
      }
      break;

    case EPState_Recv:
      tcb_queue_t queue;
      tcb_t* dest;

      // Get the head of the endpoint queue
      queue = ep_ptr_get_queue(epptr);
      dest = queue.head;
      
      // Haskell error "Receive endpoint queue must not be empty"
      //assert(dest);

      queue = tcb_ep_dequeue(dest, queue);
      ep_ptr_set_queue(epptr, queue);

      if(!queue.head){
        ep_ptr_set_state(epptr, EPState_Idle);
      }

      bool_t reply_can_grant = thread_state_ptr_get_blocking_ipc_can_grant(&dest->tcbstate);

      if(do_call){
        if(canGrant || canGrantReply){
          setup_caller_cap();
        }
        else{
          set_thread_state();
        }
      }
      break;
  }
}

void receive_ipc(tcb_t* thread, cap_t cap, boot_t isBlocking){
  endpoint_t* epptr;
  notification_t* ntfnPtr;
  
  // Haskell error
  //assert(cap_get_capType(cap) == cap_endpoint_cap)
  
  epptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

  ntfnPtr = thread->tcb_bound_notification;
  if(ntfnPtr && notification_ptr_get_state(ntfnPtr) == NtfnState_Active){

  }
  else{
    switch(enpoint_pte_get_state(epptr)){
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

        queue = ep_ptr_get_queue(epptr);
        sender = queue.head;

        // Haskell
        // assert(sender);

        queue = tcb_ep_dequeue(sender, queue);
        ep_ptr_set_queue(epptr, queue);

        if(!queue.head){
          ep_ptr_set_state(epptr, EPState_Idle);
        }

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