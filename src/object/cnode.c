#include <types.h>
#include <api/failures.h>
#include <api/types.h>
#include <object/structures.h>
#include <object/objecttype.h>
#include <object/cnode.h>
#include <object/interrupt.h>
#include <object/untyped.h>
#include <kernel/cspace.h>
#include <kernel/thread.h>
#include <util.h>

void cte_insert(Cap newCap, Cte* srcSlot, Cte* destSlot) {
    MdbNode srcMDB, newMDB;
    Cap srcCap;
    bool newCapIsRevocable;

    srcMDB = srcSlot->cteMDBNode;
    srcCap = srcSlot->cap;

    newCapIsRevocable = is_cap_revocable(newCap, srcCap);

    newMDB = srcMDB;
    newMDB.mdbPrev = CTE_REF(srcSlot);
    newMDB.mdbRevocable = newCapIsRevocable;
    newMDB.mdbFirstBadged = newCapIsRevocable;

    // panic

    set_untypedCap_as_full(srcCap, newCap, srcSlot);

    destSlot->cap = newCap;
    destSLot->cteMDBNode = newMDB;
    &srcSlot->cteMDBNode.mdbNext = CTE_REF(destSlot);
    if(newMDB.mdbNext) {
        &CTE_PTR(newMDB.mdbNext)->cteMDBNode, CTE_REF(destSlot);
    }
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

void insert_new_cap(Cte* parent, Cte* slot, Cap cap) {
    Cte* next;

    next = CTE_PTR(parent->cteMDBNode.mdbNext);
    slot->cap = cap;
    slot->cteMDBNode.mdbNext = CTE_REF(next);
    slot->cteMDBNode.mdbRevocable = true;
    slot->cteMDBNode.mdbFirstBadge = true;
    slot->cteMDBNode.mdbPrev = CTE_REF(parent);

    if(next) {
        &next->cteMDBNode = CTE_REF(slot);
    }
    &parent->cteMDNNode = CTE_REF(slot);
}