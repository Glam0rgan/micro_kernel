struct TcbQueue {
  Tcb* head;
  Tcb* end;
};

void setup_caller_cap(Tcb* sender, Tcb* receiver, bool canGrant);
TcbQueue tch_ep_append(Tcb* tcb, TcbQueue queue);
TcbQueue tcb_ep_dequeue(Tcb* tcb, TcbQueue queue);
u64 copyMRs(Tcb* sender, u64* sendBuf, Tcb* receiver,
  u64* recvBuf, u64 n);

Cptr PURE get_extra_cptr(u64* bufferPtr, u64 i);