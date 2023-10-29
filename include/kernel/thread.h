
void scheduleTCB(tcb_t* tptr);

void set_thread_state(tcb_t* tptr, _thread_state_t ts);

void possible_switch_to(tcb_t* tptr);

void doIPCTransfer(tcb_t* sender,enpoint_t endpoint,
                   u64 badge, bool_t grant, tcb_t* reveiver);