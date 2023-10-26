#include "structures.h"
#include "util.h"
#include "tcb.h"

void setup_caller_cap(tcb_t* sender, tcb_t* receiver, bool_t canGrant){

}

// Add tcb to an endpoint queue
tcb_queue_t tch_ep_append(tcb_t* tcb, tcb_queue_t queue){
  if(!queue.head)
    queue.head = tcb;
  else
    queue.end->tcbEPNext = tcb;
  tcb->tcbEPPrev = queue.end;
  tcb->tcbEPNext = NULL;
  queue.end = tcb;
}


// Remove tcb from an endpoint queue
tcb_queue_t tcb_ep_dequeue(tcb_t* tcb, tcb_queue_t queue){
  if(tcb->tcbEPPrev){
    tcb->tcbEPPrev->tcbEPNext = tcb->tcbEPNext;
  }
  else{
    queue.head = tcb->tcbEPNext;
  }

  if(tcb->tcbEPNext){
    tcb->tcbEPNext->tcbEPPrev = tcb->tcbEPPrev;
  }
  else{
    queue.end = tcb->tcbEPPrev;
  }

  return queue;
}