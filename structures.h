#include "types.h"
#include "arch/object/structures.h"

#define wordRadix 6
#define CTE_PTR(r) ((cte_t *)(r))
#define EP_PTR(r) ((endpoint_t *)(r))
typedef u64 seL4_CapRights;

// High means to calculate from the high bit

// thread state
enum _thread_state{
  ThreadState_Inactive = 0,
  ThreadState_Running,
  ThreadState_Restart,
  ThreadState_BlockedOnReceive,
  ThreadState_BlockedOnSend,
  ThreadState_BlockedOnReply,
  ThreadState_BlockedOnNotification,
  ThreadState_IdleThreadState
};
typedef u64 _thread_state_t;

typedef struct{
  u64 padding;
  u64 capType : 5;
  u64 : 5;
} null_cap_t;

typedef struct {
  u64 capEPBadge;
  u64 capType : 5;
  u64 capCanGrantReply : 1;
  u64 capCanGrant : 1;
  u64 capCanReceive : 1;
  u64 capCanSend : 1;
  u64 padding : 7;
  u64 capEPPtr : 48; // high
} endpoint_cap_t;

typedef struct{
  u64 capFreeIndex : 48;
  u64 : 9;
  u64 capIsDevice : 1;
  u64 capBlockSize : 6;
  u64 capType : 5;
  u64 : 11;
  u64 capPtr : 48; // high
  
} untyped_cap_t;

typedef struct{
  u64 capNtfnBadge;
  u64 capType : 5;
  u64 capNtfnCanReceive : 1;
  u64 capNtfnCanSend : 1;
  u64 : 9;
  u64 capNtfnPtr : 48; // high
} notification_cap_t;

typedef struct{
  u64 caoTCBPtr;

  u64 capType : 5;
  u64 : 57;
  u64 caoReplyCanGrant : 1;
  u64 canReplyMaster : 1;
} reply_cap_t;

// The user-visible format of the data word is defined by cnode_capdata, below.
typedef struct{
  u64 capCNodeGuard;
  u64 capType : 5;
  u64 capCNodeGuardSize : 6;
  u64 capCNodeRadix : 6;
  u64 capCNodePtr : 47; // high
} cnode_cap_t;

typedef struct{
  u64 padding;

  u64 capType : 5;
  u64 : 11;
  u64 capTCBPtr : 48; // high

} thread_cap_t;

typedef struct{
  u64 padding;

  u64 capType : 5;
  u64 : 59;
} irq_control_cap_t;

typedef struct{
  u64 capIRQ;

  u64 cap_Type : 5;
  u64 : 59;
} irq_handler_cap_t;

typedef struct{
  u64 capZombieID;

  u64 capType : 5;
  u64 : 52;
  u64 capZombieType : 7;
} zombie_cap_t;

typedef struct{
  u64 padding;

  u64 capType : 5;
  u64 : 59;
} domain_cap_t;

// end 

// Enpoint size = 16 bytes
typedef struct{
  u64 epQueue_head;
  u64 padding : 16;
  u64 epQueue_tail : 46; // high
  u64 state : 2;
} endpoint_t;

// Async endpoint size = 32 bytes
typedef struct{
  u64 : 16;
  u64 ntfnBoundTCB : 48; // high
  u64 ntfnMsgIdentifier;
  u64 : 16;
  u64 ntfnQueue_head : 48; // high
  u64 ntfnQueue_tail : 48; // high
  u64 : 14;
  u64 state : 2;
} notification_t;

// Mapping database node
typedef struct{
  u64 : 16;
  u64 mdbNext : 46; // high
  u64 mdbRevocable : 1;
  u64 mdbFirstBadged : 1;
  u64 mdbprev;
} mdb_node_t;

// Lookup fault: size = 16 bytes
typedef struct{
  u64 padding;

  u64 : 62;
  u64 lufType : 2;
} invalid_root_t;

typedef struct{
  u64 paddintg;

  u64 : 55;
  u64 bitsLeft : 7;
  u64 lufType : 2;
} missing_capability_t;

typedef struct{
  u64 padding;

  u64 : 48;
  u64 bitsFound : 7;
  u64 bitsLeft : 7;
  u64 lufType : 2;
} depth_mismatch_t;

typedef struct{
  u64 guardFound; 

  u64 : 48;
  u64 bitsLeft : 7;
  u64 bitsFound : 7;
  u64 lufType : 2;
} guard_mismatch_t;

enum lufType{
  invalid_root = 0,
  missing_capability,
  depth_mismatch,
  guard_mismatch
}

// Fault: size = 16 bytes
typedef struct{
  u64 padding;

  u64 : 60;
  u64 os_FaultType : 4;
} NullFault_t;

typedef struct{
  u64 address;

  u64 inReceivePhase : 1;
  u64 : 59;
  u64 os_FaultType : 4;
} CapFault_t;

typedef struct{
  u64 syscallNumber;

  u64 : 60;
  u64 os_FaultType : 4;
} UnknownSyscall_t;

typedef struct{
  u64 padding;

  u64 number : 32;
  u64 code : 28;
  u64 os_FaultType : 4;
} UserException_t;

// Thread state size = 24 bytes
typedef struct{
  u64 blocking_ipc_badge;
  u64 : 60;
  u64 blocking_ipc_canGrant : 1;
  u64 blocking_ipc_cangrantreply : 1;
  u64 blocking_ipc_iscall : 1;
  u64 tcb_queue : 1;
  u64 padding : 16;
  u64 blockingObject : 44;
  u64 tsType : 4;
} thread_state_t;

// Universal capability
typedef struct{
  u64 capEPBadge;
  u64 capEPPtr : 48;
  u64 capType : 5;
  u64 left : 59;
} cap_t;

typedef struct{
  u64 label : 52;
  u64 capsUnwrapped : 3;
  u64 extraCaps : 2;
  u64 length : 7;
} message_info_t;

struct tcb{
  // arch specific tcb state
  arch_tcb_t tcb_arch;

  // thread state 3 u64
  thread_state tcb_state;

  notification* tcb_bound_notification;

  os_fault_t tcb_fault;

  lookup_fault_t tcb_lookup_failure;

  dom_t tcb_domain;

  prio_t tcb_mcp;

  prio_t tcb_priority;

  u64 tcb_time_slice;

  cptr_t tcb_fault_handler;

  u64 tcb_ipc_buffer;

#ifdef ENABLE_SMP_SUPPORT
  u64 tcb_affinity;
#endif  
  
  struct tcb* tcb_sched_next;
  struct tcb* tcb_sched_prev;

  struct tcb* tcb_ep_next;
  struct tcb* tcb_ep_prev;
};
typedef struct tcb tcb_t;

typedef struct ipc_buffer_{
  message_info_t tag;
  u64 msg[msg_max_length];
  u64 user_data;
  u64 caps_or_badges[msg_max_extra_caps];
  cptr_t receiveCNode;
  cptr_t receiveIndex;
  cptr_t receiveDepth;
} ipc_buffer __attribute__((__aligned__(sizeof(struct ipc_buffer_))));

enum endpoint_state{
  EPState_Idle = 0,
  EPState_Send = 1,
  EPState_Recv = 2
};
typedef u64 endpoint_state;

enum cap_tag {    cap_null_cap = 0,    cap_untyped_cap = 2,    cap_endpoint_cap = 4,    
  cap_notification_cap = 6,    cap_reply_cap = 8,    
  cap_cnode_cap = 10,    cap_thread_cap = 12,    
  cap_small_frame_cap = 1,    cap_frame_cap = 3,    
  cap_asid_pool_cap = 5,    cap_page_table_cap = 7,    
  cap_page_directory_cap = 9,    cap_asid_control_cap = 11,    
  cap_irq_control_cap = 14,    cap_irq_handler_cap = 30,    
  cap_zombie_cap = 46,    cap_domain_cap = 62};

// cdt -> capability derivation tree
// Capability table entry
struct cte{
  cap_t cap;
  mdb_node_t cteMBDNode;
};
typedef struct cte cte_t;