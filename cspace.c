#include "types.h"
#include "failures.h"
#include "cspace.h"
#include "util.h"

lookupCap_ret_t lookup_cap(tch_t* thread, cptr_t cPtr){

}

lookupSlot_raw_ret_t lookup_slot(tch_t* thread, cptr_t capptr){
  
}

resolve_address_bits_ret_t resolve_address_bits(cap_t node_cap, cptr_t cap_ptr, u64 n_bits){
  resolve_address_bits_ret_t ret;
  u64 radix_bits, guard_bits, level_bits, guard;
  u64 cap_guard, offset;
  cte_t* slot;

  ret.bits_remaining = n_bits;
  ret.slot = NULL;

  if(unlikely(node_cap.capType != cap_cnode_cap)){
    // invalid_root_fault
    return ret;
  }
               
  cnode_cap_t cnode_cap = (cnode_cap_t)node_cap;

  while(1){
    radix_bits = node_cap.capCNodeRadix;
    guard_bits = node_cap.capCGuardSize;
    level_bits = radix_bits + guard_bits;

    // assert(level_bits != 0);

    cap_guard = node_cap.capCnodeGuard;

    guard = (cap_ptr >> ((n_bits - guard_bits) & MASK(wordRadix))) & MASK(guard_bits);

    if(unlikely(guard_bits > n_bits || guard != cap_guard)){
      // guard_mismatch
      return ret;
    }

    if(unlikely(level_bits > n_bits)){
      // depth_mismatch
      return ret;
    }

    offset = (cap_ptr >> (n_bits - level_bits)) & MASK(radix_bits);
    slot = CTE_PTR(node_cap.capCNodePtr) + offset;

    if(likely(n_bits <= level_bits)){
      ret.status = EXCEPTION_NONE;
      ret.slot = slot;
      ret.bits_remaining = 0;
      return ret;  
    }

    n_bits -= level_bits;
    node_cap = slot->cap;
    
    if(unlikely(node_cap.capType != cap_cnode_cap)){
      ret.status = EXCEPTION_NONE;
      ret.slot = slot;
      ret.bits_remaining = n_bits;
      return ret;
    }
  }

  ret.status = EXCEPTION_NONE;
  return ret;
}