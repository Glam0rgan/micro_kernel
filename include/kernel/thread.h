
void scheduleTCB(tcb_t* tptr);

void set_thread_state(tcb_t* tptr, _thread_state_t ts);

void possible_switch_to(tcb_t* tptr);

void do_ipc_transfer(tcb_t* sender, enpoint_t endpoint,
    u64 badge, bool_t grant, tcb_t* reveiver);
void do_normal_transfer(tcb_t* sender, u64* sendBuffer, endpoint* endpoint,
    u64 badge, bool_t canGrant, tcb_t* receiver,
    u64* receiveBuffer);
void do_fault_transfer(u64 badge, tcb_t* sender, tcb_t* reveiver,
    u64* reveiverIPCBuffer);