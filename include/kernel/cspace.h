#include "structures.h"

struct lookup_slot_raw_ret{
  exception_t status;
  cte_t* slot;
};
typedef struct lookup_slot_raw_ret lookup_slot_raw_ret_t;

struct lookup_slot_ret{
  exception_t status;
  cte_t* slot;
}
typedef struct lookup_slot_ret lookup_slot_ret_t;

struct resolve_address_bits_ret{
  exception_t status;
  cte_t* slot;
  u64 bits_remaining;
};
typedef struct resolve_address_bits_ret resolve_address_bits_ret_t;

lookup_slot_raw_ret_t lookup_slot(tcb_t* thread, cptr_t capptr); 