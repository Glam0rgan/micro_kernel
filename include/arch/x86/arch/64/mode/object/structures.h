#pragma once

#include <os/macros.h>
#ifndef u64

#define u64 uint64_t

#endif

#define GDT_NULL    0
#define GDT_CS_0    1
#define GDT_DS_0    2
#define GDT_TSS     3 //TSS is two slots in x86-64
#define GDT_CS_3    5
#define GDT_DS_3    6
#define GDT_FS      7
#define GDT_GS      8
#define GDT_ENTRIES 9

#define PML4E_PTR(r)     ((Pml4e *)(r))
#define PML4E_PTR_PTR(r) ((Pml4e **)(r))
#define PML4E_REF(p)     ((u64)(p))

#define PML4_PTR(r)     ((Pml4e *)(r))
#define PML4_REF(p)     ((u64)(r))

#define PDPTE_PTR(r)   ((Pdpte *)(r))
#define PDPTE_PTR_PTR(r) ((Pdpte **)(r))
#define PDPTE_REF(p)   ((u64)(p))

#define PDPT_PTR(r)    ((Pdpte *)(r))
#define PDPT_PREF(p)   ((u64)(p))

#define PDE_PTR(r)     ((Pde *)(r))
#define PDE_PTR_PTR(r) ((Pde **)(r))
#define PDE_REF(p)     ((u64)(p))

#define PD_PTR(r)    ((Pde *)(r))
#define PD_REF(p)    ((u64)(p))

#define PTE_PTR(r)    ((Pte *)(r))
#define PTE_REF(p)    ((u64)(p))

#define PT_PTR(r)    ((Pte *)(r))
#define PT_REF(p)    ((u64)(p))

typedef struct Trapframe_t {
  u64 eax;      // rax
  u64 rbx;
  u64 rcx;
  u64 rdx;
  u64 rbp;
  u64 rsi;
  u64 rdi;
  u64 r8;
  u64 r9;
  u64 r10;
  u64 r11;
  u64 r12;
  u64 r13;
  u64 r14;
  u64 r15;

  u64 trapno;
  u64 err;

  u64 rip;     // rip
  u64 cs;
  u64 eflags;  // rflags
  u64 rsp;     // rsp
  u64 ds;      // ss
}Trapframe;

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

/*
static inline bool CONST arch_is_cap_revocable(Cap derivedCap, Cap srcCap) {
  switch(derivedCap.capType) {
  case cap_io_port_cap:
    return srcCap.capType == cap_io_port_control_cap;

  default:
    return false;
  }
}
*/
typedef struct _PageTableCap {
  u64 : 4;
  u64 capPTMappedASID : 12;
  u64 capPTBasePtr : 48;

  u64 capType : 5;
  u64 : 9;
  u64 capPTIsMapped : 1;
  u64 capPTMappedAddress : 28;
  u64 : 21;
}PageTableCap;

typedef struct _PageDirectoryCap {
  u64 : 4;
  u64       capPDMappedASID : 12;
  u64  capPDBasePtr : 48;

  u64       capType : 5;
  u64 : 9;
  u64       capPDIsMapped : 1;
  u64  capPDMappedAddress : 19;

  u64 : 30;
}PageDirectoryCap;

typedef struct  _PdptCap {
  u64 : 4;
  u64       capPDPTMappedASID : 12;
  u64  capPDPTBasePtr : 48;

  u64       capType : 5;
  u64       capPDPTIsMapped : 1;
  u64  capPDPTMappedAddress : 10;
  u64 : 48;
}PdptCap;

typedef struct _Pml4Cap {
  u64       capPML4BasePtr : 64;

  u64       capType : 5;
  u64       capPML4IsMapped : 1;
  u64 : 46;
  u64       capPML4MappedASID : 12;
}Pml4Cap;

// PML4, PDPE, PDs and PTs, assuming 51-bit physical address

typedef struct _Pml4e {
  u64       xd : 1;
  u64 : 11;
  u64 : 1;
  u64       pdptBaseAddress : 39;
  u64 : 4;
  u64 : 1;
  u64 : 1;
  u64       accessed : 1;
  u64       cacheDisabled : 1;
  u64       writeThrough : 1;
  u64       superUser : 1;
  u64       readWrite : 1;
  u64       present : 1;
}Pml4e;

typedef struct _Pdpte {
  u64  xd : 1;
  u64 : 11;
  u64 : 1;
  u64  pdBaseAddress : 39;
  u64 : 4;
  u64       pageSize : 1;
  u64 : 1;
  u64       accessed : 1;
  u64       cacheDisabled : 1;
  u64       writeThrough : 1;
  u64       superUser : 1;
  u64       readWrite : 1;
  u64       present : 1;
}Pdpte;

typedef struct _Pde {
  u64       xd : 1;
  u64 : 11;
  u64 : 1;
  u64  ptBaseAddress : 39;
  u64 : 4;
  u64       pageSize : 1;
  u64 : 1;
  u64       accessed : 1;
  u64       cacheDisabled : 1;
  u64       writeThrough : 1;
  u64       superuser : 1;
  u64       readWrite : 1;
  u64       present : 1;
}Pde;

typedef struct _Pte {
  u64       xd : 1;
  u64 : 11;
  u64 : 1;
  u64  pageBaseAddress : 39;
  u64 : 3;
  u64       global : 1;
  u64       pat : 1;
  u64       dirty : 1;
  u64       accessed : 1;
  u64       cacheDisabled : 1;
  u64       writeThrough : 1;
  u64       superUser : 1;
  u64       readWrite : 1;
  u64       present : 1;
}Pte;

typedef struct _Cr3 {
  u64 : 13;
  u64 pml4BaseAddress : 39;
  u64       pcid : 12;
}Cr3;

typedef Pml4e VSpaceRoot;
