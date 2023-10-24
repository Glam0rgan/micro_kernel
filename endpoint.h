#include "structures.h"
#include "util.h"
#include "tcb.h"

#define ep_ptr_get_epQueue_head(a) (a->high);
#define ep_ptr_get_epQueue_tail(a) (a->low&~MASK(2));// wait for fix

#define ep_ptr_set_epQueue_head(epptr, addr) (epptr->high = addr)
#define ep_ptr_set_epQueue_tail(epptr, addr) (epptr->low &= addr) // wait for fix

#define ep_ptr_get_state(epptr) (epptr->low&MASK(2))
#define ep_ptr_set_state(epptr, num){epptr->low&=~MASK(2);epptr->low|=num;}

static inline PURE tcb_queue_t ep_ptr_get_queue(endpoint_t* epptr){
  tcb_queue_t queue;
  
  queue.head = (tcb_t *)ep_ptr_get_epQueue_head(epptr);
  queue.end = (tcb_t *)ep_ptr_get_epQueue_tail(epptr);
}

static inline void ep_ptr_set_queue(endpoint_t* epptr, tcb_queue_t queue){
  ep_ptr_set_epQueue_head(epptr, (u64)queue.head);
  ep_ptr_set_epQueue_tail(epptr, (u64)queue.end);
}