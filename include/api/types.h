#pragma once

#include <stdint.h>
#include <util.h>
#include <arch/api/types.h>
#include <arch/types.h>
#include <os/types.h>
#include <os/constants.h>

//typedef uint64_t Prio;

static inline OsMessageInfo CONST messageinfo_from_u64_raw(uint64_t w) {
  OsMessageInfo mi;

  mi = *(OsMessageInfo*)(&w);
  return mi;
}

static inline OsMessageInfo CONST messageinfo_from_u64(uint64_t w) {
  OsMessageInfo mi;
  uint64_t len;

  // Cast to OsMessageInfo
  mi = *(OsMessageInfo*)(&w);

  // Fix the length.
  len = mi.length;
  if(len > Os_MsgMaxLength) {
    mi.length = Os_MsgMaxLength;
  }

  return mi;
}

typedef struct _CapTransfer {
  CPtr ctReceiveRoot;
  CPtr ctReceiveIndex;
  uint64_t ctReceiveDepth;
}CapTransfer;

static inline CapTransfer PURE capTransfer_from_u64(uint64_t* wptr)  {
  CapTransfer transfer;
  transfer.ctReceiveRoot = (CPtr)wptr[0];
  transfer.ctReceiveIndex = (CPtr)wptr[1];
  transfer.ctReceiveDepth = wptr[2];

  return transfer;
}

static inline uint64_t CONST u64_from_messageinfo(OsMessageInfo mi) {
  uint64_t tmp = *(uint64_t*)(&mi);
  return tmp;
}
