#include "types.h"

#define wordRadix 6
#define CTE_PTR(r) ((cte_t *)(r))
#define EP_PTR(r) ((endpoint_t *)(r))
typedef u64 seL4_CapRights;

struct two_u64_structs{
  u64 high;
  u64 low;
};

struct three_u64_structs{
  u64 high;
  u64 mid;
  u64 low;
}

struct four_u64_structs{
  u64 high_high; // the highest address
  u64 high_low;
  u64 low_high;
  u64 low_low;   // the lowest address
};

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

/*
  struct notification{
    padding 16
    ntfnBoundTCB 48

    ntfnMsgIdentifier 64

    padding 16
    ntfnQueue_head 48

    ntfnQueue_tail 48
    padding 14

    state 2
  }

  struct cnode_cap{
    capCNodeGuard 64
    capType 5
    capCNodeGuardSize 6
    capCNodeRadix 6
    capCNodePtr 47
  }

  struct mdb_node{
    padding 16
    mdbnext 46
    mdbrevocable 1
    mdbfirst badeged 1
    mdbprev 64
  }

  struct endpoint_cap{
    capEPBadge 64
    capType 5
    capCanGrantReply 1
    capCanGrant 1
    capCanSend 1
    padding 7
    capEPPtr 48
  }

  struct endpoint{
    epQueue_head 64
    padding 16
    epQueue_tail 46
    state 2
  }

  struct message_info{
    label 52
    capsUnwrapped 3
    extraCaps 2
    length 7
  }
*/
typedef u64 message_info;

typedef struct two_u64_structs cnode_cap_t;
typedef struct two_u64_structs endpoint_cap_t;
typedef struct two_u64_structs endpoint_t;
typedef struct two_u64_structs mdb_node_t;
typedef struct two_u64_structs cap_t;


typedef struct four_u64_structs notification_t;



/*
struct thread_state{
    blocking_ipc_badge 64
    padding 60
    blocking_ipc_canGrant 1
    blocking_ipc_cangrantreply 1
    blocking_ipc_iscall 1
    tcb_queue 1
    padding 16
    blockingObject 44
    tsType 4
};

*/
typedef struct three_u64_structs thread_state_t;
#define thread_state_ptr_set_tsType(tcbState, state) 
#define thread_state_ptr_set_blockingObject(tcbState, blockingObject)
#define  

struct tcb{
  // arch specific tcb state
  arch_tcb_t tcb_arch;

  // thread state 3 u64
  thread_state_t tcb_state;

  notification_t* tcb_bound_notification;

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
  message_info tag;
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
typedef u64 endpoint_state_t;

struct cap{    
  u64 words[2];
};

enum cap_tag {    cap_null_cap = 0,    cap_untyped_cap = 2,    cap_endpoint_cap = 4,    
  cap_notification_cap = 6,    cap_reply_cap = 8,    
  cap_cnode_cap = 10,    cap_thread_cap = 12,    
  cap_small_frame_cap = 1,    cap_frame_cap = 3,    
  cap_asid_pool_cap = 5,    cap_page_table_cap = 7,    
  cap_page_directory_cap = 9,    cap_asid_control_cap = 11,    
  cap_irq_control_cap = 14,    cap_irq_handler_cap = 30,    
  cap_zombie_cap = 46,    cap_domain_cap = 62};

struct cte{
  cap_t cap;
  mdb_node_t cteMBDNode;
};
typedef struct cte cte_t;