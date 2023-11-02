
// Use endpoint pointer get the thread queue.
static inline TcbQueue PURE epptr_get_queue(Endpoint* epptr) {
  TcbQueue queue;

  queue.head = (Tcb*)epptr.epQueue_head;
  queue.end = (Tcb*)epptr.epQueue_tail;

  return queue;
}


// Set the thread queue.
static inline void epptr_set_queue(Endpoint* epptr, TcbQueue queue) {
  epptr->epQueue_head = (u64)queue.head;
  epptr->epQueue_head = (u64)queue.end;
}