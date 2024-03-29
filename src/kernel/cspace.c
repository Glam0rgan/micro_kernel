#include <types.h>
#include <object.h>
#include <api/failures.h>
#include <kernel/thread.h>
#include <kernel/cspace.h>
#include <model/statedata.h>
#include <arch/machine.h>

// Invoke lookup_slot to get slot.
// Return cap.
LookupCapRet lookup_cap(Tcb* thread, CPtr cPtr) {
  LookupSlotRawRet luRet;
  LookupCapRet ret;

  luRet = lookup_slot(thread, cPtr);
  if(unlikely(luRet.status != EXCEPTION_NONE)) {
    ret.status = luRet.status;
    ret.cap.capType = cap_null_cap;
    return ret;
  }

  ret.status = EXCEPTION_NONE;
  ret.cap = luRet.slot->cap;
  return ret;
}

// Invoke lookup_slot to get slot.
// Return cap and slot.
LookupCapAndSlotRet lookup_cap_and_slot(Tcb* thread, CPtr cPtr) {
  LookupSlotRawRet luRet;
  LookupCapAndSlotRet ret;

  luRet = lookup_slot(thread, cPtr);
  if(unlikely(luRet.status != EXCEPTION_NONE)) {
    ret.status = luRet.status;
    ret.cap.capType = cap_null_cap;
    return ret;
  }

  ret.status = EXCEPTION_NONE;
  ret.cap = luRet.slot->cap;
  ret.slot = luRet.slot;
  return ret;
}

// Look-up slot by thread and capptr.
// Return slot pointer.
LookupSlotRawRet lookup_slot(Tcb* thread, CPtr capptr) {
  Cap threadRoot;
  ResolveAddressBitsRet resRet;
  LookupSlotRawRet ret;

  // Get the cnode_cap for the root cnode address.
  threadRoot = TCB_PTR_CTE_PTR(thread, tcbCTable)->cap;
  CNodeCap* cnode = (CNodeCap*)&threadRoot;
  //cprintf("cnode %l\n", (cnode->capCNodePtr<<1) | 0xFFFF800000000000);
  ret.slot = ((cnode->capCNodePtr << 1) | 0xFFFF800000000000) + capptr;
   
  // Get the status and concrete slot
  //resRet = resolve_address_bits(threadRoot, capptr, wordBits);

  //ret.status = resRet.status;
  //ret.slot = resRet.slot;
  return ret;
}

// IsSource may mean whether gets the true slot or a copy.
LookupSlotRet lookup_slot_for_cnode_op(bool isSource, Cap root, CPtr capptr, u64 depth) {
  ResolveAddressBitsRet resRet;
  LookupSlotRet ret;
  ret.slot = NULL;

  if(unlikely(root.capType != cap_cnode_cap)) {
    currentSyscallError.type = OsFailedLookup;
    currentSyscallError.failedLookupWasSource = isSource;
    currentLookupFault.lufType = invalidRoot;
    ret.status = EXCEPTION_SYSCALL_ERROR;
    return ret;
  }

  if(unlikely(depth < 1 || depth > wordBits)) {
    currentSyscallError.type = OsRangeError;
    currentSyscallError.rangeErrorMin = 1;
    currentSyscallError.rangeErrorMax = wordBits;
    ret.status = EXCEPTION_SYSCALL_ERROR;
    return ret;
  }

  resRet = resolve_address_bits(root, capptr, depth);
  if(unlikely(resRet.status != EXCEPTION_NONE)) {
    currentSyscallError.type = OsFailedLookup;
    currentSyscallError.failedLookupWasSource = isSource;
    ret.status = EXCEPTION_SYSCALL_ERROR;
    return ret;
  }

  if(unlikely(resRet.bitsRemaining != 0)) {
    currentSyscallError.type = OsFailedLookup;
    currentSyscallError.failedLookupWasSource = isSource;
    // Need fix.
    currentLookupFault.type = depthMismatch;
    currentLookupFault.bitsRemaining = resRet.bitsRemaining;
    ret.status = EXCEPTION_SYSCALL_ERROR;
    return ret;
  }

  ret.slot = resRet.slot;
  ret.status = EXCEPTION_NONE;
  return ret;
}

LookupSlotRet lookup_target_slot(Cap root, CPtr capptr, u64 depth) {
  return lookup_slot_for_cnode_op(false, root, capptr, depth);
}

LookupSlotRet lookup_source_slot(Cap root, CPtr capptr, u64 depth) {
  return lookup_slot_for_cnode_op(true, root, capptr, depth);
}

LookupSlotRet lookup_pivot_slot(Cap root, CPtr capptr, u64 depth) {
  return lookup_slot_for_cnode_op(true, root, capptr, depth);
}

// Resolve address and get the status and slot 
// nBits is 1 << 6 in 64-bits.
// go to the manual to see concrete process.
ResolveAddressBitsRet resolve_address_bits(Cap nodeCap, CPtr capPtr, u64 nBits) {
  ResolveAddressBitsRet ret;
  u64 radixBits, guardBits, levelBits, guard;
  u64 capGuard, offset;
  Cte* slot;

  ret.bitsRemaining = nBits;
  ret.slot = NULL;

  if(unlikely(nodeCap.capType != cap_cnode_cap)) {
    // invalid_root_fault
    return ret;
  }

  // Cast to cnode_cap             
  CNodeCap cnodeCap = (*(CNodeCap*)(&nodeCap));

  // resolve the address
  radixBits = cnodeCap.capCNodeRadix;
  guardBits = cnodeCap.capCNodeGuardSize;
  levelBits = radixBits + guardBits;

  // The MASK(wordRadix) here is to avoid the case where
  // nBits = wordBits (=2^wordRadix) and guardBits = 0, as it violates
  // the C spec to shift right by more than wordBits-1.

  if(unlikely(levelBits > nBits)) {
    // depth_mismatch
    return ret;
  }

  // Simplify the address.
  offset = capPtr;
  slot = CTE_PTR(cnodeCap.capCNodePtr << 1) + offset;
  levelBits = 0;

  if(likely(levelBits == 0)) {
    ret.status = EXCEPTION_NONE;
    ret.slot = slot;
    ret.bitsRemaining = 0;
    return ret;
  }

  nodeCap = slot->cap;
  cnodeCap = (*(CNodeCap*)(&nodeCap));

  if(unlikely(nodeCap.capType != cap_cnode_cap)) {
    ret.status = EXCEPTION_NONE;
    ret.slot = slot;
    ret.bitsRemaining = nBits;
    return ret;
  }

  ret.status = EXCEPTION_NONE;
  return ret;
}
