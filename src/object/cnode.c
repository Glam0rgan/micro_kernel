#include <api/types.h>
#include <types.h>
#include <util.h>
#include <api/failures.h>
#include <object/structures.h>
#include <object/objecttype.h>
#include <object/cnode.h>
#include <object/interrupt.h>
#include <object/untyped.h>
#include <kernel/cspace.h>
#include <kernel/thread.h>

void cte_insert(Cap newCap, Cte* srcSlot, Cte* destSlot) {
    MdbNode srcMDB, newMDB;
    Cap srcCap;
    bool newCapIsRevocable = false;

    srcMDB = srcSlot->cteMdbNode;
    srcCap = srcSlot->cap;

    //newCapIsRevocable = is_cap_revocable(newCap, srcCap);

    newMDB = srcMDB;
    newMDB.mdbPrev = CTE_REF(srcSlot);
    newMDB.mdbRevocable = newCapIsRevocable;
    newMDB.mdbFirstBadged = newCapIsRevocable;

    // panic

    //set_untypedCap_as_full(srcCap, newCap, srcSlot);

    destSlot->cap = newCap;
    destSlot->cteMdbNode = newMDB;
    srcSlot->cteMdbNode.mdbNext = CTE_REF(destSlot);
    if(newMDB.mdbNext) {
        (CTE_PTR(newMDB.mdbNext)->cteMdbNode).mdbPrev = CTE_REF(destSlot);
    }
}

CapTransfer PURE load_cap_transfer(u64* buffer) {
    const int offset = Os_MsgMaxLength + Os_MsgMaxExtraCaps + 2;
    return capTransfer_from_u64(buffer + offset);
}

// This implementation is specialised to the (current) limit
// of one cap receive slot. 
Cte* get_receive_slots(Tcb* thread, u64* buffer) {
    CapTransfer ct;
    CPtr cptr;
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

    lucRet = lookup_cap(thread, cptr);
    if(lucRet.status != EXCEPTION_NONE) {
        return NULL;
    }
    cnode = lucRet.cap;

    // 
    lusRet = lookup_target_slot(cnode, ct.ctReceiveIndex, ct.ctReceiveDepth);
    if(lusRet.status != EXCEPTION_NONE) {
        return NULL;
    }
    slot = lusRet.slot;

    if(slot->cap.capType != cap_null_cap) {
        return NULL;
    }

    return slot;
}

void insert_new_cap(Cte* parent, Cte* slot, Cap cap) {
    //Cte* next;
    //cprintf("%l\n", slot);
    //panic("insert_new_cap");
    //next = CTE_PTR(parent->cteMdbNode.mdbNext);
    //panic("%l",slot->cap.capType);
    slot->cap = cap;
    //slot->cteMdbNode.mdbNext = CTE_REF(next);
    //slot->cteMdbNode.mdbRevocable = true;
    //slot->cteMdbNode.mdbFirstBadged = true;
    //slot->cteMdbNode.mdbPrev = CTE_REF(parent);

    //if(next) {
    //    next->cteMdbNode.mdbPrev = CTE_REF(slot);
    //}
    //parent->cteMdbNode.mdbNext = CTE_REF(slot);
}
