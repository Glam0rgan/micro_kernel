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

static inline u64 CONST u64_from_messageinfo(OsMessageInfo mi) {
  return (u64)mi;
}