#include "structures.h"
#include "util.h"
#include "tcb.h"

static inline tcb_queue_t PURE epptr_get_queue(endpoint_t* epptr) {
  tcb_queue_t queue;

  queue.head = (tcb_t*)epptr.epQueue_head;
  queue.end = (tcb_t*)epptr.epQueue_tail;

  return queue;
}

static inline void epptr_set_queue(endpoint_t* epptr, tcb_queue_t queue) {
  epptr->epQueue_head = (u64)queue.head;
  epptr->epQueue_head = (u64)queue.end;
}