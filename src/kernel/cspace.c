#include <types.h>
#include <ctypes.h>
#include <failures.h>
#include <structures.h>
#include <cspace.h>
#include <util.h>

// Invoke lookup_slot to get slot.
// Return cap.
LookupCapRet lookup_cap(Tcb* thread, Cptr cPtr) {
  LookupSlotRawRet luRet;
  LookupCapRet ret;

  luRet = lookup_slot(thread, cPtr);
  if(unlikely(luRet.status != Exception_NONE)) {
    ret.status = luRet.status;
    ret.cap.capType = cap_null_cap;
    return ret;
  }

  ret.status = EXCEPTION_NONE;
  ret.cap = luRet.slot->cap;
  return ret;
}
// Look-up slot by thread and capptr.
// Return slot pointer.
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

// IsSource may mean whether gets the true slot ro a copy.
LookupSlotRet lookup_slot_for_cnode_op(boot isSource, Cap root, cptr capptr, u64 depth) {
  ResolveAddressBitsRet resRet;
  LookupSlotRet ret;
  ret.slot = NULL;

  if(unlikely(root.capType != cap_cnode_cap)) {
    currentSyscallError.type = os_FailedLookup;
    currentSyscallError.failedLookupWasSource = isSource;
    currentLookupFault.lufType = invalidRoot;
    ret.status = EXCEPTION_SYSCALL_ERROR;
    return ret;
  }

  if(unlikely(depth < 1 || depth > wordBits)) {
    currentSyscallError.type = os_RangeError;
    currentSyscallError.rangeErrorMin = 1;
    currentSyscallError.rangeErrorMax = wordBits;
    ret.status = EXCEPTION_SYSCALL_ERROR;
    return ret;
  }

  resRet = resolve_address_bits(root, capptr, depth);
  if(unlikely(resRet.status != EXCEPTION_NONE) {
    currentSyscallError.type = os_FailedLookup;
    currentSyscallError.failedLookupWasSource = isSource;
    ret.status = EXCEPTION_SYSCALL_ERROR;
    return ret;
  }

  if(unlikely(resRet.bitsRemaining != 0)) {
    currentSyscallError.type = os_FailedLookup;
    currentSyscallError.failedLookupWasSource = isSource;
    // Need fix.
      currentLookupFault.type = Lookup_fault_depth_mismatch;
    currentLookupFault.bitsRemaning = resRet.bitsRemaining;
    ret.status = EXCEPTION_SYSCALL_ERROR;
    return ret;
  }

  ret.slot = resRet.slot;
  ret.status = EXCEPTION_NONE;
  return ret;
}

LookupSlotRet lookup_target_slot(Cap root, Cptr capptr, u64 depth) {
  return lookup_slot_for_cnode_op(false, root, capptr, depth);
}

// Resolve address and get the status and slot 
// n_bits is 1 << 6 in 64-bits.
// go to the manual to see concrete process.
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