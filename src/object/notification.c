#include <types.h>
#include <kernel/thread.h>
#include <object/structures.h>
#include <object/tcb.h>
#include <object/endpoint.h>
#include <model/statedata.h>
#include <machine/io.h>

#include <object/notification.h>

static inline TcbQueue PURE ntfn_ptr_get_queue(Notification* ntfnPtr) {
    TcbQueue ntfnQueue;

    ntfnQueue.head = (Tcb*)ntfnPtr->ntfnQueueHead;
    ntfnQueue.head = (Tcb*)ntfnPtr->ntfnQueueTail;

    return ntfnQueue;
}

static inline void ntfn_ptr_set_queue(Notification* ntfnPtr, TcbQueue ntfnQueue) {
    ntfnPtr->ntfnQueueHead = (u64)ntfnQueue.head;
    ntfnPtr->ntfnQueueTail = (u64)ntfnQueue.Tail;
}

void complete_signal(Notification* ntfnPtr, Tcb* tcb) {
    u64 badge;

    if(likely(tcb && ntfnPtr->state == NtfnState_Active)) {
        badge = ntfnPtr->ntfnMsgIdentifier;
        set_register(tcb, badgeRegister, badge);
        ntfnPtr->state = NtfnState_Idle;
    } else {
        // panic
    }
}