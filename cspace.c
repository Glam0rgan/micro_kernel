#include "failures.h"
#include "types.h"
#include "cspace.h"
#include "util.h"

u64 cap_cnode_cap_get_capCNodeRadix(cap_t node_cap){
  return (node_cap.low >> 47) & MASK(6);  
}

u64 cap_cnode_cap_get_capCGuardSize(cap_t node_cap){
  return (node_cap.low >> 53) & MASK(6);  
}

u64 cap_cnode_cap_get_capCnodeGuard(cap_t node_cap){
  return node_cap.high;
}

u64 cap_cnode_cap_get_capCNodePtr(cap_t node_cap){
  return node_cap.low & MASK(47);
}

u64 cap_get_capType(cap_t cap){
  return cap.low >> 59;
}

resolve_address_bits_ret_t resolve_address_bits(cap_t node_cap, cptr_t cap_ptr, u64 n_bits){
  resolve_address_bits_ret_t ret;
  u64 radix_bits, guard_bits, level_bits, guard;
  u64 cap_guard, offset;
  cte_t* slot;

  ret.bits_remaining = n_bits;
  ret.slot = NULL;

  if(unlikely(cap_get_capType(node_cap) != cap_cnode_cap)){
    // invalid_root_fault
    return ret;
  }

  while(1){
    radix_bits = cap_cnode_cap_get_capCNodeRadix(node_cap);
    guard_bits = cap_cnode_cap_get_capCGuardSize(node_cap);
    level_bits = radix_bits + guard_bits;

    // assert(level_bits != 0);

    cap_guard = cap_cnode_cap_get_capCnodeGuard(node_cap);

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
    slot = CTE_PTR(cap_cnode_cap_get_capCNodePtr(node_cap)) + offset;

    if(likely(n_bits <= level_bits)){
      ret.status = EXCEPTION_NONE;
      ret.slot = slot;
      ret.bits_remaining = 0;
      return ret;  
    }

    n_bits -= level_bits;
    node_cap = slot->cap;
    
    if(unlikely(cap_get_capType(node_cap) != cap_cnode_cap)){
      ret.status = EXCEPTION_NONE;
      ret.slot = slot;
      ret.bits_remaining = n_bits;
      return ret;
    }
  }

  ret.status = EXCEPTION_NONE;
  return ret;
}