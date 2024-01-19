#pragma once

#include <types.h>
#include <util.h>
#include <object/structures.h>
#include <arch/machine.h>

Tcb* ksSchedulerAction;

/*
static inline CONST u64 ready_queues_index(u64 dom, u64 prio) {
    if(numDomains > 1) {
        return dom * CONFIG_NUM_PRIORITIES + prio;
    } else {
        // if(dom == 0)
        //   panic();
        return prio;
    }
}
*/

void schedule_tcb(Tcb* tptr);

void set_thread_state(Tcb* tptr, _ThreadState ts);

void possible_switch_to(Tcb* tptr);
void reschedule_required(void);
void schedule(void);

void do_ipc_transfer(Tcb* sender, Endpoint endpoint,
    u64 badge, bool grant, Tcb* reveiver);
void do_normal_transfer(Tcb* sender, u64* sendBuffer, Endpoint* endpoint,
    u64 badge, bool canGrant, Tcb* receiver,
    u64* receiveBuffer);
void do_fault_transfer(u64 badge, Tcb* sender, Tcb* reveiver,
    u64* reveiverIPCBuffer);

void set_threadState(Tcb* tptr, _ThreadState ts);
void choose_thread(void);

static inline bool PURE is_runnable(const Tcb* thread) {
    switch(thread->tcbState.tsType) {
    case ThreadState_Running:
    case ThreadState_Restart:
        return true;
    default:
        return false;
    }
}

static inline bool PURE is_stopped(const Tcb* thread) {
    switch(thread->tcbState.tsType) {
    case ThreadState_Inactive:
    case ThreadState_BlockedOnReceive:
    case ThreadState_BlockedOnSend:
    case ThreadState_BlockedOnNotification:
    case ThreadState_BlockedOnReply:
        return true;
    default:
        return false;
    }
}

static inline bool PURE is_blocking(const Tcb* thread) {
    switch(thread->tcbState.tsType) {
    case ThreadState_BlockedOnReceive:
    case ThreadState_BlockedOnSend:
    case ThreadState_BlockedOnNotification:
    case ThreadState_BlockedOnReply:
        return true;
    default:
        return false;
    }
}

#define isSchedulable isRunnable