#include <config.h>
#include <types.h>
#include <api/failures.h>
#include <api/syscall.h>
#include <arch/object/objecttype.h>
#include <machine/io.h>
#include <object/objecttype.h>
#include <object/structures.h>
#include <object/notification.h>
#include <object/endpoint.h>
#include <object/cnode.h>
#include <object/interrupt.h>
#include <object/tcb.h>
#include <object/untyped.h>
#include <model/statedata.h>
#include <kernel/thread.h>
#include <kernel/vspace.h>
#include <machine.h>
#include <util.h>
#include <string.h>

u64 get_object_size(u64 objectType, u64 userObjectSize) {
    // arch_get_object_size

    switch(objectType) {
    case osTCBObject:
        return OsTCBBits;
    case osEndpointObject:
        return OsEndpointBits;
    case osNotificationObject:
        return OsNotificationBits;
    case osCapTableObject:
        return OsSlotBits + userObjectSize;
    case osUntypedObject:
        return userObjectSize;
    default:
        panic("invalid object type");
        return 0;
    }
}

Cap create_object(u64 objectType, void* regionBase, u64 userSize, bool deviceMemory) {
    // wait to implement the function arch_create_object
    /*
    // Handle architecture-specific objects.
    if(t >= (u64)osNonArchObjectTypeCount) {
        return arch_create_object(objectType, regionBase, userSize, deviceMemory);
    }
    */
    switch(objectType) {
    case osTCBObject: {
        Tcb* tcb;
        tcb = TCB_PTR((u64)regionBase + TCB_OFFSET);

        tcb->tcbTimeSlice = CONFIG_TIME_SLICE;
        // tcbDomain
        ThreadCap threadCap;
        threadCap.capType = cap_thread_cap;
        threadCap.capTCBPtr = TCB_REF(tcb);
    }
    case osEndpointObject: {
        EndpointCap endpointCap;
        endpointCap.capCanGrant = 1;
        endpointCap.capCanGrantReply = 1;
        endpointCap.capCanReceive = 1;
        endpointCap.capCanSend = 1;
        endpointCap.capEPPtr = EP_REF(regionBase);
        endpointCap.capType = cap_endpoint_cap;
        Cap cap = *(Cap*)(&endpointCap);
        return cap;
    }
    case osNotificationObject: {
        NotificationCap notificationCap;
        notificationCap.capNtfnCanReceive = 1;
        notificationCap.capNtfnCanSend = 1;
        notificationCap.capType = cap_notification_cap;
        notificationCap.capNtfnPtr = NTFN_REF(regionBase);
        Cap cap = *(Cap*)(&notificationCap);
        return cap;
    }
    case osCapTableObject: {
        CNodeCap cnodeCap;
        cnodeCap.capCNodeGuard = userSize;
        cnodeCap.capCNodeGuardSize = 0;
        cnodeCap.capCNodeRadix = 0;
        cnodeCap.capCNodePtr = CTE_REF(regionBase);
        Cap cap = *(Cap*)(&cnodeCap);
        return cap;
    }
    case osUntypedObject: {
        UntypedCap untypedCap;
        untypedCap.capFreeIndex = 0;
        untypedCap.capIsDevice = !!deviceMemory;
        untypedCap.capBlockSize = userSize;
        untypedCap.capPtr = (u64)regionBase;
        untypedCap.capType = cap_untyped_cap;
        Cap cap = *(Cap*)(&untypedCap);
        return cap;
    }
    default: {
        panic("createobjectdefault");
    }
    }
}

void create_new_object(u64 objectType, Cte* parent,
    Cte* destCNode, u64 destOffset, u64 destLength,
    void* regionBase, u64 userSize, bool deviceMemory) {
    u64 objectSize;
    void* nextFreeArea;
    u64 i;

    objectSize = get_object_size(objectType, userSize);

    nextFreeArea = regionBase;
    for(i = 0;i < destLength;i++) {
        //Create the object.
        Cap cap = create_object(objectType, (void*)((u64)nextFreeArea + (i << objectSize)), userSize, deviceMemory);

        // Insert the cap into the user's cspace.
        insert_new_cap(parent, &destCNode[destOffset + i], cap);
    }
}

u64 CONST cap_get_cap_size_bits(Cap cap) {
    CapTag cTag;

    cTag = cap.capType;

    switch(cTag) {
    case cap_untyped_cap:
        UntypedCap untypedCap = *(UntypedCap*)(&cap);
        return untypedCap.capBlockSize;

    case cap_endpoint_cap:
        return OsEndpointBits;

    case cap_notification_cap:
        return OsNotificationBits;

    case cap_cnode_cap:
        CNodeCap cNodeCap = *(CNodeCap*)(&cap);
        return cNodeCap.capCNodeRadix + OsSlotBits;

    case cap_thread_cap:
        return OsTCBBits;

    case cap_zombie_cap: {
        ZombieCap zombieCap = *(ZombieCap*)(&cap);
        u64 type = zombieCap.capZombieType;
        if(type == ZombieType_ZombieTCB) {
            return OsTCBBits;
        }
        // need fix
        return ZombieType_ZombieCNode(type) + OsSlotBits;
    }

    case cap_null_cap:
        return 0;

    case cap_domain_cap:
        return 0;

    case cap_reply_cap:
        return 0;

    case cap_irq_control_cap:
        return 0;

    case cap_irq_handler_cap:
        return 0;

    default:
        // fix
        return cap_get_archCapSizeBits(cap);
    }
}

bool CONST is_cap_revocable(Cap derivedCap, Cap srcCap) {
    if(is_arch_cap(derivedCap)) {
        return arch_is_cap_revocable(derivedCap, srcCap);
    }
    switch(derivedCap.capType) {
    case cap_endpoint_cap:
        EndpointCap endpointDerivedCap = *(EndpointCap*)(&derivedCap);
        EndpointCap endpointSrcCap = *(EndpointCap*)(&srcCap);
        return endpointDerivedCap.capEPBadge !=
            endpointSrcCap.capEPBadge;
    case cap_notification_cap:
        NotificationCap ntfnDerivedCap = *(NotificationCap*)(*derivedCap);
        NotificationCap ntfnSrcCap = *(NotificationCap*)(*srcCap);
        return ntfnDerivedCap.capNtfnBadge !=
            ntfnSrcCap;
    case cap_irq_handler_cap:
        return srcCap.capType == cap_irq_control_cap;

    case cap_untyped_cap:
        return true;

    default:
        return false;
    }
}