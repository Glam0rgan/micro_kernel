#pragma once

#include <types.h>
#include <api/failures.h>
#include <api/types.h>
#include <object/structures.h>
struct _LookupCapRet {
  Exception status;
  Cap cap;
};
typedef struct _LookupCapRet LookupCapRet;

struct _LookupCapAndSlotRet {
  Exception status;
  Cap cap;
  Cte* slot;
};
typedef struct _LookupCapAndSlotRet LookupCapAndSlotRet;
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

struct _ResolveAddressBitsRet {
  Exception status;
  Cte* slot;
  u64 bitsRemaining;
};
typedef struct _ResolveAddressBitsRet ResolveAddressBitsRet;

LookupCapRet lookup_cap(Tcb* thread, CPtr cPtr);
LookupCapAndSlotRet lookup_cap_and_slot_ret(Tch* thread, CPtr cPtr);
LookupSlotRawRet lookup_slot(Tcb* thread, CPtr capptr);
