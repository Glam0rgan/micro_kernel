
void schedule_tcb(Tcb* tptr);

void set_thread_state(Tcb* tptr, _ThreadState ts);

void possible_switch_to(Tcb* tptr);

void do_ipc_transfer(Tcb* sender, Endpoint endpoint,
    u64 badge, bool grant, Tcb* reveiver);
void do_normal_transfer(Tcb* sender, u64* sendBuffer, Endpoint* endpoint,
    u64 badge, bool canGrant, Tcb* receiver,
    u64* receiveBuffer);
void do_fault_transfer(u64 badge, Tcb* sender, Tcb* reveiver,
    u64* reveiverIPCBuffer);

static inline bool PURE is_runnable(const Tcb* thread) {
    switch(thread->tcbState.tsType) {
    case ThreadState_Running:
    case ThreadState_Restart:
        return true;
    default:
        return false;
    }
}