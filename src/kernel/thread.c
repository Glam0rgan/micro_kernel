#include "types.h"
#include "thread.h"
#include "structures.h"
#include "statedata.h"

void do_ipc_transfer(tcb_t* sender, endpoint_t* endpoint,
                     u64 badge, bool_t grant, tcb_t* receiver){
  void *receiveBuffer, *sendBuffer;
  
  receiveBuffer = lookup_ipc_buffer(true, receiver);  
}

void schedule_tcb(tcb_t* tptr){
  if(tptr == NODE_STATE(ksCurThread) &&
     NODE_STATE(ksSchedulerAction) == SchedulerAction_ResumCurrentThread &&
     !isSchedulable(tptr)){
     rescheduleRequired();
  }
}

void set_thread_state(tcb_t* tptr, _thread_state_t ts){

}

void possible_switch_to(tcb_t* tptr){
    
}