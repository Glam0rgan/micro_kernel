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