#include "structures.h"
#include "tcb.h"

// Remove tcb from an endpoint queue
tcb_queue_t tcb_ep_dequeue(tcb_t* tcb, tcb_queue_t queue){
  if(tcb->tcb_ep_prev){
    tcb->tcb_ep_prev->tcb_ep_next = tcb->tcb_ep_next;
  }
  else{
    queue.head = tcb->tcb_ep_next;
  }

  if(tcb->tcb_ep_next){
    tcb->tcb_ep_next->tcb_ep_prev = tcb->tcb_ep_prev;
  }
  else{
    queue.end = tcb->tcb_ep_prev;
  }

  return qeueue;
}