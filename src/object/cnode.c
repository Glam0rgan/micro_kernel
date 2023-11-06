#include <types.h>
#include <ctypes.h>
#include "cnode.h"
#include "failures.h"

exception_t decode_cnode_invocation() {

}

exception_t invoke_cnode_revoke() {

}

exception_t invoke_cnode_delete() {

}

exception_t invoke_cnode_cancel_badged_sends() {

}

exception_t invoke_cnode_insert() {

}

exception_t invoke_cnode_move() {

}

exception_t invoke_cnode_rorate() {

}

exception_t invoke_cnode_save_caller() {

}

static void set_untyped_cap_as_full() {

}

void cte_insert(Cap newCap, Cte* srcSlot, Cte* destSlot) {
    MdbNode srcMDB, newMDB;
    Cap srcCap;
    bool newCapIsRevocable;

    srcMDB = srcSlot->cteMDBNode;
    srcCap = srcSlot->cap;

    newCapIsRevocable = is_cap_revocable(newCap, srcCap);

    newMDB;

    // panic

    set_untypedCap_as_full(srcCap, newCap, srcSlot);

    destSlot->cap = newCap;
    destSLot->cteMDBNode = newMDB;


}
// This implementation is specialised to the (current) limit
// of one cap receive slot. 
Cte* get_receive_slots(Tcb* thread, u64* buffer) {
    CapTransfer ct;
    Cptr cptr;
    LookupCapRet lucRet;
    LookupSlotRet lusRet;
    Cte* slot;
    Cap cnode;

    if(!buffer) {
        return NULL;
    }

    // Use receiver buffer to get receiver slot info
    ct = load_cap_transfer(buffer);
    cptr = ct.ctReceiveRoot;

    lucRet = lookupCap(thread, cptr);
    if(lucRet.status != EXCEPTION_NONE) {
        return NULL;
    }
    cnode = lucRet.cap;

    // 
    lusRet = lookupTargetSlot(cnode, ct.ctReceiveIndex, ct.ctReceiveDepth);
    if(lusRet.status != EXCEPTION_NONE) {
        return NULL;
    }
    slot = lusRet.slot;

    if(slot->cap.capType != cap_null_cap) {
        return NULL;
    }

    return slot;
}

Cap_Tranfer PURE load_cap_transfer(u64* buffer) {
    const int offset = os_MsgMaxLength + os_MsgMaxExtraCaps + 2;
    return capTransfer_from_u64(buffer + offset);
}