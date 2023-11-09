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