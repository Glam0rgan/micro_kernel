#pragma once

#include <types.h>
#include <object/structures.h>

void complete_signal(Notification* ntfnPtr, Tcb* tcb);
void send_signal(Notification* ntfnPtr, u64 badge);
void receive_signal(Tcb* thread, Cap cap, bool isBlocking);
void cancel_signal(Tcb* threadPtr, Notification* ntfnPtr);
void cancel_allSignals(Notification* ntfnPtr);
void bind_notification(Tcb* tcb, Notification* ntfnPtr);
void unbind_notification(Tcb* tcb);
void unbind_maybe_notification(Notification* ntfnPtr);

static inline void ntfn_set_active(Notification* ntfnPtr, u64 badge) {
    ntfnPtr->state = NtfnState_Active;
    ntfnPtr->ntfnMsgIdentifier = badge;
}
