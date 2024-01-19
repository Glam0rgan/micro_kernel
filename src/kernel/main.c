#include "types.h"
#include "defs.h"
#include "mmu.h"
#include "memlayout.h"
#include "proc.h"
#include <objecttype.h>
#include <cspace.h>
#include <structures.h>

void ipc_test(void) {


  // Alloc the cte pointer and tcb pointer memory.
  void* tcbObject0 = memblock_alloc_kernel(PGSIZE, PGSIZE);
  void* tcbObject1 = memblock_alloc_kernel(PGSIZE, PGSIZE);

  Tcb* tcbPlugin0 = (Tcb*)((char*)tcbObject0 + BIT(TCB_SIZE_BITS));
  Tcb* tcbPlugin1 = (Tcb*)((char*)tcbObject1 + BIT(TCB_SIZE_BITS));

  void* cnodePtrPlugin0 = memblock_alloc_kernel(PGSIZE, PGSIZE);
  void* cnodePtrPlugin1 = memblock_alloc_kernel(PGSIZE, PGSIZE);

  // Add to scheduler.
  tcb_sched_append(tcbPlugin0);
  tcb_sched_append(tcbPlugin1);

  // Use to create cdt.
  Cte* cteRoot = memblock_alloc_kernel(PGSIZE, PGSIZE);



  // Init the cteRoot.
  UntypedCap untypedCap = *(UntypedCap*)(&cteRoot->cap);
  untypedCap.capPtr = memblock_alloc_kernel(PGSIZE, PGSIZE);
  cteRoot->cap = *(Cap*)(&untypedCap);
  cteRoot->cteMdbNode.mdbPrev = NULL;
  cteRoot->cteMdbNode.mdbNext = NULL;

  // Init the CNodeCap.
  create_new_object(osCapTableObject, cteRoot,
    TCB_PTR_CTE_PTR(tcbPlugin0, tcbCTable), 0, 1,
    cnodePtrPlugin0, 0, 0);

  create_new_object(osCapTableObject, cteRoot,
    TCB_PTR_CTE_PTR(tcbPlugin1, tcbCTable), 0, 1,
    cnodePtrPlugin1, 0, 0);

  // Allocate caps by root_task (need capdl).
  // Create endpoint and endpointcap.
  Endpoint* endpointTest = memblock_alloc_kernel(PGSIZE, PGSIZE);
  Cap capTest = create_object(osEndpointObject, endpointTest, 0ul, false);

  // Insert cap to the cspace.
  insert_new_cap(cteRoot,
    lookup_slot(tcbPlugin0, 0).slot,
    capTest);

  insert_new_cap(cteRoot,
    lookup_slot(tcbPlugin1, 0).slot,
    capTest);

}

int main(void) {

  // Init kernel.
  vm_init();
  memblock_init();
  lapic_init();
  ioapic_init();
  console_init();

  char*** pluginArgv;
  load_plugin(pluginArgv);

  ipc_test();

  // 
  schedule();
  panic("ok");
}
