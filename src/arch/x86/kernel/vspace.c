#include <types.h>
#include <util.h>
#include <arch/machine/hardware.h>
#include <object/structures.h>
#include <kernel/vspace.h>
#include <constants.h>

// Use tcb to get 
u64* PURE lookup_ipc_buffer(bool isReceiver, Tcb* thread) {
  u64 w_bufferPtr;
  Cap bufferCap;
  enum VmRights vmRights;

  // Get tcbIPCBuffer pointer
  w_bufferPtr = thread->tcbIPCBuffer;
  // Get tcbBuffer capability
  bufferCap = TCB_PTR_CTE_PTR(thread, tcbBuffer)->cap;

  // the capType must be frame
  if(unlikely(bufferCap.capType != cap_frame_cap)) {
    return NULL;
  }

  // Cast universal cap to frame cap
  FrameCap* frameCapPtr = (FrameCap*)(&bufferCap);
  FrameCap frameCap = *frameCapPtr;

  // the frame cap must not be device
  //if(unlikely(frameCap.capFIsDevice)) {
  //  return NULL;
  //}

  vmRights = frameCap.capFVMRights;
  // If the thread is sender, need VMReadOnly
  // If the thread is receiver, need VMReadWrite
  if(vmRights == VMReadWrite || (!isReceiver && vmRights == VMReadOnly)) {
    u64 basePtr, pageBits;
    basePtr = frameCap.capFBasePtr;
    pageBits = pageBits_forSize(frameCap.capFSize);
    return (u64*)(basePtr + (w_bufferPtr & MASK(pageBits)));
  } else {
    return NULL;
  }
}
