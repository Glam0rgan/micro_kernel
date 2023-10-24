#include "structures.h"

struct tcb_queue{
  tcb_t* head;
  tcb_t* end;
};

typedef struct tcb_queue tcb_queue_t;

tcb_queue_t tch_ep_append(tcb_t* tcb, tcb_queue_t queue);
tcb_queue_t tcb_ep_dequeue(tcb_t* tcb, tcb_queue_t queue);

