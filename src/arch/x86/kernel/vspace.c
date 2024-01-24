#include <types.h>
#include <util.h>
#include <object/structures.h>
#include <kernel/vspace.h>
#include <constants.h>
// Use tcb to get 
u64* PURE lookup_ipc_buffer(bool_t isReceiver, tcb_t* thread) {
  u64 w_bufferPtr;
  cap_t bufferCap;
  vm_rights_t vm_rights;

  // Get tcbIPCBuffer pointer
  w_bufferPtr = thread->tcbIPCBuffer;
  // Get tcbBuffer capability
  bufferCap = TCB_PTR_CTE_PTR(thread, tcbBuffer)->cap;

  // the capType must be frame
  if(unlikely(frame_cap.capType != cap_frame_cap)) {
    return NULL;
  }

  // Cast universal cap to frame cap
  frame_cap_t* frame_cap_ptr = (frame_cap_t*)(&buffercap);
  frame_cap_t frame_cap = *frame_cap_ptr;

  // the frame cap must not be device
  if(unlikely(frame_cap.capFIsDevice)) {
    return NULL;
  }

  vm_rights = frame_cap.capFVMRights;
  // If the thread is sender, need VMReadOnly
  // If the thread is receiver, need VMReadWrite
  if(vm_rights == VMReadWrite || (!isReceiver && vm_rights == VMReadOnly)) {
    u64 basePtr, pageBits;
    basePtr = frame_cap.capFBasePtr;
    pageBits = pageBitsForSize(frame_cap.capFSize);
    return (u64*)(basePtr + (w_bufferPtr & MASK(pageBits)));
  } else {
    return NULL;
  }
}
