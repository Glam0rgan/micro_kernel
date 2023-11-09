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

u64 CONST cap_get_cap_size_bits(Cap cap) {
    CapTag cTag;

    cTag = cap.capType;

    switch(cTag) {
    case cap_untyped_cap:
        UntypedCap untypedCap = *(UntypedCap*)(&cap);
        return untypedCap.capBlockSize;

    case cap_endpoint_cap:
        return os_EndpointBits;

    case cap_notification_cap:
        return os_NotificationBits;

    case cap_cnode_cap:
        CNodeCap cNodeCap = *(CNodeCap*)(&cap);
        return cNodeCap.capCNodeRadix + os_SlotBits;

    case cap_thread_cap:
        return os_TCBBits;

    case cap_zombie_cap: {
        ZombieCap zombieCap = *(ZombieCap*)(&cap);
        u64 type = zombieCap.capZombieType;
        if(type == ZombieType_ZombieTCB) {
            return os_TCBBits;
        }
        // need fix
        return ZombieType_ZombieCNode(type) + os_SlotBits;
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