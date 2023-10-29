#include "types.h"
#include "util.h"
#include "structures.h"
#include "vspace.h"
#include "constants.h"

u64* PURE lookup_ipc_buffer(bool_t isReceiver, tcb_t* thread){
  u64 w_bufferPtr;
  cap_t bufferCap;
  vm_rights_t vm_rights;

  w_bufferPtr = thread->tcbIPCBuffer;
  bufferCap = TCB_PTR_CTE_PTR(thread, tcbBuffer)->cap;
  
  // the capType must be small_frame or frame
  if(unlikely(bufferCap.capType!=cap_small_frame_cap&&
              bufferCap.capType!=cap_frame_cap)){
    return NULL;
  }

  if(unlikely()){
    return NULL;
  }

  vm_rights = ;
  if(likely()){

  }
}