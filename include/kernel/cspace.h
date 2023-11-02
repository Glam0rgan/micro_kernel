
struct LookupSlotRawRet {
  Exception status;
  Cte* slot;
};

struct LookupSlotRet {
  Exception status;
  Cte* slot;
};

struct ResolveAddressBitsRet {
  Exception status;
  Cte* slot;
  u64 bitsRemaining;
};

LookupSlotRawRet lookup_slot(Tcb* thread, Cptr capptr);