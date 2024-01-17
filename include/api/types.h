#pragma once

#include <stdint.h>
#include <util.h>
#include <arch/api/types.h>
#include <arch/types.h>
#include <os/constants.h>

typedef u64 Prio;

static inline OsMessageInfo CONST messageinfo_from_u64_raw(u64 w) {
  OsMessageInfo mi;

  mi = (OsMessageInfo)w;
  return mi;
}

static inline OsMessageInfo CONST messageinfo_from_u64(u64 w) {
  OsMessageInfo mi;
  u64 len;

  // Cast to OsMessageInfo
  mi = (OsMessageInfo)w;

  // Fix the length.
  len = mi.length;
  if(len > OsMsgMaxLength) {
    mi.length = OsMsgMaxLength;
  }

  return mi;
}

struct _CapTransfer {
  Cptr ctReceiveRoot;
  Cptr ctReceiveIndex;
  u64 ctReceiveDepth;
};
typedef struct _CapTranfer CapTransfer;

static inline CapTransfer PURE capTransfer_from_u64(u64* wptr) {
  CapTransfer transfer;
  transfer.ctReceiveRoot = (Cptr)wptr[0];
  transfer.ctReceiveIndex = (Cptr)wptr[1];
  transfer.ctReceiveDepth = wptr[2];

  return transfer;
}

static inline u64 CONST u64_from_messageinfo(OsMessageInfo mi) {
  return (u64)mi;
}