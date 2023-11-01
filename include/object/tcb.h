struct tcb_queue{
  tcb_t* head;
  tcb_t* end;
};

typedef struct tcb_queue tcb_queue_t;

void setup_caller_cap(tcb_t* sender, tcb_t* receiver, bool_t canGrant);
tcb_queue_t tch_ep_append(tcb_t* tcb, tcb_queue_t queue);
tcb_queue_t tcb_ep_dequeue(tcb_t* tcb, tcb_queue_t queue);
u64 copyMRs(tcb_t* sender, u64* sendBuf, tcb_t* receiver,
            u64* recvBuf, u64 n);

cptr_t PURE get_extra_cptr(u64* bufferPtr, u64 i);