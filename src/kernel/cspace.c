#include <types.h>
#include <ctypes.h>
#include <failures.h>
#include <structures.h>
#include <cspace.h>
#include <util.h>

// Look-up slot by thread and capptr
LookupSlotRawRet lookup_slot(tcb_t* thread, cptr_t capptr) {
  Cap threadRoot;
  ResolveAddressBitsRet resRet;
  LookupSlotRawRet ret;

  // Get the cnode_cap for the root cnode address.
  threadRoot = TCB_PTR_CTE_PTR(thread, tcbCTable)->cap;

  // Get the status and concrete slot
  resRet = resolve_address_bits(threadRoot, capptr, wordBits);

  ret.status = resRet.status;
  ret.slot = resRet.slot;
  return ret;
}

// Resolve address and get the status and slot 
// n_bits is 1 << 6 in 64-bits.
// go to the manual to see concrete process
ResolveAddressBitsRet resolve_address_bits(Cap node_cap, Cptr cap_ptr, u64 n_bits) {
  ResolveAddressBitsRet ret;
  u64 radix_bits, guard_bits, level_bits, guard;
  u64 cap_guard, offset;
  Cte* slot;

  ret.bits_remaining = n_bits;
  ret.slot = NULL;

  if(unlikely(node_cap.capType != cap_cnode_cap)) {
    // invalid_root_fault
    return ret;
  }

  // Cast to cnode_cap             
  CNodeCap cnode_cap = (*(CNodeCap*)(&node_cap));

  // resolve the address
  while(1) {
    radix_bits = node_cap.capCNodeRadix;
    guard_bits = node_cap.capCGuardSize;
    level_bits = radix_bits + guard_bits;

    // assert(level_bits != 0);

    cap_guard = node_cap.capCnodeGuard;

    guard = (cap_ptr >> ((n_bits - guard_bits) & MASK(wordRadix))) & MASK(guard_bits);

    if(unlikely(guard_bits > n_bits || guard != cap_guard)) {
      // guard_mismatch
      return ret;
    }

    if(unlikely(level_bits > n_bits)) {
      // depth_mismatch
      return ret;
    }

    offset = (cap_ptr >> (n_bits - level_bits)) & MASK(radix_bits);
    slot = CTE_PTR(node_cap.capCNodePtr) + offset;

    if(likely(n_bits <= level_bits)) {
      ret.status = EXCEPTION_NONE;
      ret.slot = slot;
      ret.bits_remaining = 0;
      return ret;
    }

    n_bits -= level_bits;
    node_cap = slot->cap;

    if(unlikely(node_cap.capType != cap_cnode_cap)) {
      ret.status = EXCEPTION_NONE;
      ret.slot = slot;
      ret.bits_remaining = n_bits;
      return ret;
    }
  }

  ret.status = EXCEPTION_NONE;
  return ret;
}