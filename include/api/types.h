static inline OsMessageInfo CONST messageinfo_from_u64(u64 w) {
  OsMessageInfo mi;
  u64 len;

  // Cast to OsMessageInfo
  mi = (Os_MessageInfo)w;

  // Fix the length.
  len = mi.length;
  if(len > os_MsgMaxLength) {
    mi.length = os_MsgMaxLength;
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