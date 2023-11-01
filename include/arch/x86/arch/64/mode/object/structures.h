// x86-64 specific cap types
typedef struct frame_cap {
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
} frame_cap_t;