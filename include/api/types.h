static inline os_messageinfo_t CONST messageInfo_from_u64(u64 w){
  os_messageinfo_t mi;
  u64 len;
  
  // Cast to os_message_info
  mi = (os_message_info_t)w;
  
  // Fix the length.
  len = mi.length;
  if (len > os_MsgMaxLength){
    mi.length = os_MsgMaxLength;
  }

  return mi;
}