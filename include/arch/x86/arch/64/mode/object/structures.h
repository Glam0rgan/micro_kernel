// x86-64 specific cap types
struct _FrameCap {
  // The ASID doubles as the PCI bus/dev/fun when used in an IOMMU context
  //so it is 16 instead of 12
  u64 capFMappedASID : 16;
  u64 capFBasePtr : 48;

  u64 capType : 5;
  u64 capFSize : 2;
  u64 capFMapType : 2;
  u64 capFMappedAddress : 48;
  u64 capFVMRights : 2;
  u64 caoFIsDevice : 1;
  u64 : 4;
};
typedef struct _FrameCap FrameCap;

static inline bool CONST arch_is_cap_revocable(Cap derivedCap, Cap srcCap) {
  switch(derivedCap.capType) {
  case cap_io_port_cap:
    return srcCap.capType == cap_io_port_control_cap;

  default:
    return false;
  }
}