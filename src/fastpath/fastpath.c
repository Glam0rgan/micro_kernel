#include <config.h>
#include <fastpath/fastpath.h>

#ifdef CONFIG_ARCH_ARM
static inline
FORCE_INLINE
#endif
void NORETURN fastpath_call(u64 cptr, u64 msgInfo) {
    OsMessageInfo info;
    EndpointCap epCap;
    Endpoint* epPtr;
    u64 length;
    Tcb* dest;
    u64 badge;
    Cap newVTable;
    VspaceRoot* capPd;
    Pde storedHwAsid;
    u64 faultType;
    Dom dom;

    // Get message info, length, and fault type
    info = messageinfo_from_u64_raw(msgInfo);
    length = info.length;
    faultType = NODE_STATE(ksCurThread->tcbFault);

    // Check there's no extra caps, the length is ok and there
    // is no save fault.
    if(unlikely(fastpath_mi_check(msgInfo) ||
        faultType != Os_Fault_NullFault)) {
        slowpath(Syscall);
    }

    // Lookup the cap.
    Cap cap = lookup_fp(TCB_PTR_CTE_PTR(NODE_STATE(ksCurThread), tcbCTable)->cap, cptr);
    Endpoint* endpointPtr = &cap;
    epCap = *endpointPtr;

    // Check it's an endpoint.
    if(unlikely(!cap_capType_equals(cap, cap_endpoint_cap)) ||
        !epCap.capCanSend) {
        slowpath(Syscall);
    }

    // Get the endpoint address.
    epPtr = EP_PTR(epCap.capEPPtr);


    // Get the destination thread, which is only going to be valid.
    // if the endpoint is valid.
    dest = TBC_PTR(epptr_get_queue(epPtr).head);

    // Check that there's a thread waiting to receive.
    if(unlikely(epPtr->state != EPState_Recv)) {
        slowpath(Syscall);
    }

    // Get destination thread.
    newVTable = TCB_PTR_CTE_PTR(dest, tcbVTable)->cap;

    // Get vspace root.


    // Ensure that the destination has a valid VTable.
    if(unlikely()) {

    }

#ifdef CONFIG_ARCH_X86_64
    // Borrow the storedHwAsid for PCID.
#endif

    // Let gcc optimise this out for 1 domain.
    dom = maxDom ? ksCurDomain : 0;
    // Ensure only the idle thread or lower prio threads are present in scheduler.

    // Ensure that the endpoint has has grant or grant-reply 
}