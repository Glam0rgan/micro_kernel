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
    ntfnPtr->ntfnQueueTail = (u64)ntfnQueue.end;
}

void send_signal(Notification* ntfnPtr, u64 badge) {
    switch(ntfnPtr->state) {
    case NtfnState_Idle: {
        Tcb* tcb = (Tcb*)ntfnPtr->ntfnBoundTCB;
        // Check if we are bound and that thread is waiting for a message.
        if(tcb) {
            if(tcb->tcbState.tsType == ThreadState_BlockedOnReceive) {
                // Send and start thread running.
                cancel_ipc(tcb);
                tcb->tcbState.tsType = ThreadState_Running;
                set_register(tcb, badgeRegister, badge);
                possible_switch_to(tcb);
            }
        } else {
            /* In particular, this path is taken when a thread
            is waiting on a reply cap since BlockedOnReply
            would also trigger this path. I.e, a thread
            with a bound notification will not be awakened
            by signals on that bound notification if it is
            in the middle of an OsCall.
            */
            // ???????
            ntfn_set_active(ntfnPtr, badge);
        }
    }
    case NtfnState_Waiting: {
        TcbQueue ntfnQueue;
        Tcb* dest;

        ntfnQueue = ntfn_ptr_get_queue(ntfnPtr);
        dest = ntfnQueue.head;

        // if(dest == 0)
        // panic();

        // Dequeue TCB.
        ntfnQueue = tcb_ep_dequeue(dest, ntfnQueue);
        ntfn_ptr_set_queue(ntfnPtr, ntfnQueue);

        // Set he thread state to idle if the queue is empty.
        if(!ntfnQueue.head) {
            ntfnPtr->state = NtfnState_Idle;
        }

	dest->tcbState.tsType = ThreadState_Running;
        set_register(dest, badgeRegister, badge);
        possible_switch_to(dest);
        break;
    }
    case NtfnState_Active: {
        u64 badge2;

        badge2 = ntfnPtr->ntfnMsgIdentifier;
        badge2 |= badge;

        ntfnPtr->ntfnMsgIdentifier = badge2;
        break;
    }
    }
}

void receive_signal(Tcb* thread, Cap cap, bool isBlocking) {
    Notification* ntfnPtr;

    NotificationCap notificationCap = *(NotificationCap*)(&cap);
    ntfnPtr = NTFN_PTR(notificationCap.capNtfnPtr);

    switch(ntfnPtr->state) {
    case NtfnState_Idle:
    case NtfnState_Waiting: {
        TcbQueue ntfnQueue;

        if(isBlocking) {
            // Block thread on notification object.
            thread->tcbState.tsType = ThreadState_BlockedOnNotification;
            thread->tcbState.blockingObject = NTFN_REF(ntfnPtr);
            schedule_tcb(thread);

            // Enqueue TCB.
            ntfnQueue = ntfn_ptr_get_queue(ntfnPtr);
            ntfnQueue = tcb_ep_append(thread, ntfnQueue);

            ntfnPtr->state = NtfnState_Waiting;
            ntfn_ptr_set_queue(ntfnPtr,ntfnQueue);
        } else {
            //do_nbrecv_failed_transfer(thread);
        }
        break;
    }
    case NtfnState_Active:
        set_register(thread, badgeRegister,
            ntfnPtr->ntfnMsgIdentifier);
        break;
    }
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

void cancel_allSignals(Notification* ntfnPtr) {
    if(ntfnPtr->state == NtfnState_Waiting) {
        Tcb* thread = TCB_PTR(ntfnPtr->ntfnQueueHead);

        ntfnPtr->state = NtfnState_Idle;
        ntfnPtr->ntfnQueueHead = 0;
        ntfnPtr->ntfnQueueTail = 0;

        // Set all waiting threads to Restart.
        for(;thread;thread = thread->tcbEPNext) {
            thread->tcbState.tsType = ThreadState_Restart;
            // Add thread to scheduler queue.
            SCHED_ENQUEUE(thread);
        }
        reschedule_required();
    }
}

void cancel_signal(Tcb* threadPtr, Notification* ntfnPtr) {
    TcbQueue ntfnQueue;

    // Error "cancel_signal: notification object must be in a waiting" state
    // panic();

    // Dequeue TCB.
    ntfnQueue = ntfn_ptr_get_queue(ntfnPtr);
    ntfnQueue = tcb_ep_dequeue(threadPtr, ntfnQueue);
    ntfn_ptr_set_queue(ntfnPtr, ntfnQueue);

    // Make notification object idel.
    if(!ntfnQueue.head) {
        ntfnPtr->state = NtfnState_Idle;
    }

    // Make thread inactive.
    set_threadState(threadPtr, ThreadState_Inactive);
}

// Clear ntfnBoundTCB and tcbBoundNotification.
static inline void do_unbind_notification(Notification* ntfnPtr, Tcb* tcbptr) {
    ntfnPtr->ntfnBoundTCB = (u64)0;
    tcbptr->tcbBoundNotification = NULL;
}

// Use notification pointer to unbind.
void unbind_maybe_notification(Notification* ntfnPtr) {
    Tcb* boundTCB;
    boundTCB = (Tcb*)(ntfnPtr->ntfnBoundTCB);

    if(boundTCB) {
        do_unbind_notification(ntfnPtr, boundTCB);
    }
}

// Use tcb pointer to unbind.
void unbind_notification(Tcb* tcb) {
    Notification* ntfnPtr;
    ntfnPtr = tcb->tcbBoundNotification;

    if(ntfnPtr) {
        do_unbind_notification(ntfnPtr, tcb);
    }
}

// Bind tcb and notification.
void bind_notification(Tcb* tcb, Notification* ntfnPtr) {
    ntfnPtr->ntfnBoundTCB = (u64)tcb;
    tcb->tcbBoundNotification = ntfnPtr;
}
