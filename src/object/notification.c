#include <types.h>
#include <kernel/thread.h>
#include <object/structures.h>
#include <object/tcb.h>
#include <object/endpoint.h>
#include <model/statedata.h>
#include <machine/io.h>

#include <object/notification.h>

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