
struct _LookupCapRet {
  Exception status;
  Cap cap;
};
typedef struct _LookupCapRet LookupCapRet;
struct _LookupSlotRawRet {
  Exception status;
  Cte* slot;
};
typedef struct _LookupSlotRawRet LookupSlotRawRet;

struct _LookupSlotRet {
  Exception status;
  Cte* slot;
};
typedef struct _LookupSlotRet LookupSlotRet;

struct ResolveAddressBitsRet {
  Exception status;
  Cte* slot;
  u64 bitsRemaining;
};
typedef struct _ResolveAddressBitsRet ResolveAddressBitsRet;

LookupSlotRawRet lookup_slot(Tcb* thread, Cptr capptr);