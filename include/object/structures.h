#pragma once

#include <stdint.h>
#include <api/types.h>
#include <os/os_arch/constants.h>
#include <os/constants.h>
#include <arch/object/structures.h>
#include <mode/object/structures.h>
#include <kernel/proc.h>

#define wordRadix 6

#define NTFN_PTR(r) ((Notification *)(r))
#define NTFN_REF(p) ((u64)(p))

#define CTE_PTR(r) ((Cte *)(r))
#define CTE_REF(p) ((u64)(p))

#define EP_PTR(r) ((Endpoint *)(r))
#define EP_REF(r) ((u64)(r))

// We would like the actual 'tcb' region (the portion that contains the tcb_t) of the tcb
// to be as large as possible, but it still needs to be aligned. As the TCB object contains
// two sub objects the largest we can make either sub object whilst preserving size alignment
// is half the total size. To halve an object size defined in bits we just subtract 1
//
// A diagram of a TCB kernel object that is created from untyped:
//  _______________________________________
// |     |             |                   |
// |     |             |                   |
// |cte_t|   unused    |       tcb_t       |
// |     |(debug_tcb_t)|                   |
// |_____|_____________|___________________|
// 0     a             b                   c
// a = tcbCNodeEntries * sizeof(cte_t)
// b = BIT(TCB_SIZE_BITS)
// c = BIT(OsTCBBits)

#define TCB_PTR(r)       ((Tcb *)(r))
#define TCB_CTE_PTR(r,i) (((Cte *)(r))+(i))
#define TCB_REF(p)       ((u64)(p))

// Generate a cte_t pointer from a tcb_t pointer
#define TCB_PTR_CTE_PTR(p,i)\
  (((Cte *)((u64)(p)&~MASK(OsTCBBits)))+(i))

#define TCB_SIZE_BITS (OsTCBBits - 1)
#define TCB_OFFSET    BIT(TCB_SIZE_BITS)

typedef u64 OsCapRights;

// High means to calculate from the high bit

// thread state
enum _ThreadState {
  ThreadState_Inactive = 0,
  ThreadState_Running,
  ThreadState_Restart,
  ThreadState_BlockedOnReceive,
  ThreadState_BlockedOnSend,
  ThreadState_BlockedOnReply,
  ThreadState_BlockedOnNotification,
  ThreadState_IdleThreadState
};

// A TCB CNode and a TCB are always allocated togethor, and addjacently.
// The CNode comes first.
enum TcbCNodeIndex {
  // CSpace root 
  tcbCTable = 0,

  // VSpace root
  tcbVTable = 1,

  // Reply cap slot
  tcbReply = 2,

  // TCB of most recent IPC sender
  tcbCaller = 3,

  // IPC buffer cap slot
  tcbBuffer = 4,

  tcbCNodeEntries,
};

struct _NullCap {
  u64 padding;
  u64 capType : 5;
  u64 : 5;
};
typedef struct _NullCap NullCap;
typedef struct _EndpointCap {
  u64 capEPBadge;
  u64 capType : 5;
  u64 capCanGrantReply : 1;
  u64 capCanGrant : 1;
  u64 capCanReceive : 1;
  u64 capCanSend : 1;
  u64 : 7;
  u64 capEPPtr : 48; // high
} EndpointCap;

typedef struct _UntypedCap {
  u64 capFreeIndex : 48;
  u64 : 9;
  u64 capIsDevice : 1;
  u64 capBlockSize : 6;
  u64 capType : 5;
  u64 : 11;
  u64 capPtr : 48; // high

} UntypedCap;

typedef struct _NotificationCap {
  u64 capNtfnBadge;
  u64 capType : 5;
  u64 capNtfnCanReceive : 1;
  u64 capNtfnCanSend : 1;
  u64 : 9;
  u64 capNtfnPtr : 48; // high
}NotificationCap;

typedef struct _ReplyCap {
  u64 caoTCBPtr;

  u64 capType : 5;
  u64 : 57;
  u64 caoReplyCanGrant : 1;
  u64 canReplyMaster : 1;
}ReplyCap;

// The user-visible format of the data word is defined by cnode_capdata, below.
typedef struct _CnodeCap {
  u64 capCNodeGuard;
  u64 capType : 5;
  u64 capCNodeGuardSize : 6;
  u64 capCNodeRadix : 6;
  u64 capCNodePtr : 47; // high
}CNodeCap;

typedef struct _ThreadCap {
  u64 padding;

  u64 capType : 5;
  u64 : 11;
  u64 capTCBPtr : 48; // high

}ThreadCap;

typedef struct _IrqControlCap {
  u64 padding;

  u64 capType : 5;
  u64 : 59;
}IrqControlCap;

typedef struct _IrqHandlerCap {
  u64 capIRQ;

  u64 capType : 5;
  u64 : 59;
}_IrqHandlerCap;

typedef struct _ZombieCap {
  u64 capZombieID;

  u64 capType : 5;
  u64 : 52;
  u64 capZombieType : 7;
}ZombieCap;

typedef struct _DomainCap {
  u64 padding;

  u64 capType : 5;
  u64 : 59;
}DomainCap;

// end 

// Enpoint size = 16 bytes
typedef struct _Endpoint {
  u64 epQueueHead;
  u64 padding : 14;
  u64 epQueueTail : 48; // high
  u64 state : 2;
}Endpoint;

// Async endpoint size = 32 bytes
typedef struct _Notification {
  u64 : 16;
  u64 ntfnBoundTCB : 48; // high
  u64 ntfnMsgIdentifier;
  u64 : 16;
  u64 ntfnQueueHead : 48; // high
  u64 ntfnQueueTail : 48; // high
  u64 : 14;
  u64 state : 2;
}Notification;

// Mapping database node
typedef struct _MdbNode {
  u64 : 16;
  u64 mdbNext : 46; // high
  u64 mdbRevocable : 1;
  u64 mdbFirstBadged : 1;
  u64 mdbPrev;
}MdbNode;

// Lookup fault: size = 16 bytes
typedef struct _InvalidRoot {
  u64 padding;

  u64 : 62;
  u64 lufType : 2;
}InvalidRoot;

typedef struct _MissingCapability {
  u64 paddintg;

  u64 : 55;
  u64 bitsLeft : 7;
  u64 lufType : 2;
}MissingCapability;

typedef struct _DepthMismatch {
  u64 padding;

  u64 : 48;
  u64 bitsFound : 7;
  u64 bitsLeft : 7;
  u64 lufType : 2;
}DepthMismatch;

typedef struct _GuardMismatch {
  u64 guardFound;

  u64 : 48;
  u64 bitsLeft : 7;
  u64 bitsFound : 7;
  u64 lufType : 2;
}GuardMismatch;

enum OsFaultType {
  NullFault = 0,
  CapFault = 1,
  UnkownSyscall = 2,
  UserException = 3,
  VMFault = 4
};

enum LookupFaultType {
  invalidRoot = 0,
  missingCapability,
  depthMismatch,
  guardMismatch
};

typedef struct LookupFault_t{
  enum LookupFaultType type;
  enum LookupFaultType lufType;
  u64  bitsRemaining;
}LookupFault;

// Fault: size = 16 bytes
struct _NullFault {
  u64 padding;

  u64 : 60;
  u64 osFaultType : 4;
};

struct _CapFault {
  u64 address;

  u64 inReceivePhase : 1;
  u64 : 59;
  u64 osFaultType : 4;
};

struct _UnknownSyscall {
  u64 syscallNumber;

  u64 : 60;
  u64 osFaultType : 4;
};

struct _UserException {
  u64 padding;

  u64 number : 32;
  u64 code : 28;
  u64 osFaultType : 4;
};

typedef struct _OsFault{
  u64 num;
  
  u64 padding : 60;
  u64 osFaultType : 4;
}OsFault;

// Thread state size = 24 bytes
typedef struct __ThreadState {
  u64 blockingIPCBadge;
  u64 : 60;
  u64 blockingIPCCanGrant : 1;
  u64 blockingIPCCangrantreply : 1;
  u64 blockingIPCIscall : 1;
  u64 tcbQueued : 1;
  u64 padding : 16;
  u64 blockingObject : 44;
  u64 tsType : 4;
}ThreadState;

// Universal capability
typedef struct _Cap {
  u64 capEPBadge;
  u64 capEPPtr : 48;
  u64 capType : 5;
  u64 left : 59;
}Cap;

// Thread control block
struct _Tcb {
  // arch specific tcb state
  ArchTcb tcbArch;

  // thread state 3 u64
  ThreadState tcbState;

  Notification* tcbBoundNotification;

  OsFault tcbFault;

  LookupFault tcbLookupFailure;

  Trapframe* tf;
  
  Context* context;

  u64 size;
  
  u64* pml4;
	
  char* kStack;

  u64 tcbPriority;

  u64 tcbTimeSlice;

  u64 tcbFaultHandler;

  u64* tcbIPCBuffer;

  struct _Tcb* tcbSchedNext;
  struct _Tcb* tcbSchedPrev;

  struct _Tcb* tcbEPNext;
  struct _Tcb* tcbEPPrev;
};
typedef struct _Tcb Tcb;
typedef struct _IpcBuffer {
  OsMessageInfo tag;
  u64 msg[Os_MsgMaxLength];
  u64 userData;
  u64 capsOrBadges[Os_MsgMaxExtraCaps];
  u64 receiveCNode;
  u64 receiveIndex;
  u64 receiveDepth;
}IpcBuffer __attribute__((__aligned__(sizeof(struct _IpcBuffer))));


enum EndpointState {
  EPState_Idle = 0,
  EPState_Send = 1,
  EPState_Recv = 2
};

enum NotificationState {
  NtfnState_Idle = 0,
  NtfnState_Waiting = 1,
  NtfnState_Active = 2
};

enum CapTag {
  cap_null_cap = 0, cap_untyped_cap = 2, cap_endpoint_cap = 4,
  cap_notification_cap = 6, cap_reply_cap = 8,
  cap_cnode_cap = 10, cap_thread_cap = 12,
  cap_small_frame_cap = 1, cap_frame_cap = 3,
  cap_asid_pool_cap = 5, cap_page_table_cap = 7,
  cap_page_directory_cap = 9, cap_asid_control_cap = 11,
  cap_irq_control_cap = 14, cap_irq_handler_cap = 30,
  cap_zombie_cap = 46, cap_domain_cap = 62
};

// cdt -> capability derivation tree
// Capability table entry
typedef struct _Cte {
  Cap cap;
  MdbNode cteMdbNode;
}Cte;

/*static inline u64 CONST generic_frame_cap_get_capFIsDevice(cap_t cap) {
  CapTag ctag;

  ctag = cap.capType;
  if(ctag != cap_small_frame_cap &&
       ctag != cap_frame_cap)
      panic();

if(ctag == cap_small_frame_cap) {
  return cap_small_frame_cap_get_capFIsDevice(cap);
} else {
  return cap_frame_cap_get_capFIsDevice(cap);
}
}*/
