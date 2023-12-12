#include "types.h"
#include "defs.h"
#include "mmu.h"
#include "memlayout.h"
#include "proc.h"
#include <objecttype.h>
#include <cspace.h>

void ipc_test(void) {

  // Alloc the cte pointer and tcb pointer memory.
  void* tcbPlugin0 = memblock_alloc_kernel(PGSIZE, PGSIZE);
  void* tcbPlugin1 = memblock_alloc_kernel(PGSIZE, PGSIZE);

  void* cnodePtrPlugin0 = memblock_alloc_kernel(PGSIZE, PGSIZE);
  void* cnodePtrPlugin1 = memblock_alloc_kernel(PGSIZE, PGSIZE);

  Endpoint* endpointTest = memblock_alloc_kernel(PGSIZE, PGSIZE);

  Cte* cteRoot = memblock_alloc_kernel(PGSIZE, PGSIZE);

  create_new_object(osCapTableObject, cteRoot,
    TCB_PTR_CTE_PTR(threadPlugin0, tcbCTable), 0, 1,
    cnodePtrPlugin0, 0, 0);

  create_new_object(osCapTableObject, cteRoot,
    TCB_PTR_CTE_PTR(threadPlugin0, tcbCTable), 0, 1,
    cnodePtrPlugin1, 0, 0);

  UntypedCap untypedCap = *(UntypedCap*)(&cteRoot->cap);
  untypedCap.capPtr = memblock_alloc_kernel(PGSIZE, PGSIZE);
  cteRoot->cap = *(Cap*)(&untypedCap);
  cteRoot->cteMdbNode.mdbPrev = NULL;
  cteRoot->cteMdbNode.mdbNext = NULL;

  CNodeCap* cNodeCapPlugin0 = (CNodeCap*)(&(TCB_PTR_CTE_PTR(threadPlugin0, tcbCTable)->cap));
  CNodeCap* cNodeCapPlugin1 = (CNodeCap*)(&(TCB_PTR_CTE_PTR(threadPlugin1, tcbCTable)->cap));

  cNodeCapPlugin0->capCNodePtr = memblock_alloc_kernel(PGSIZE, PGSIZE) >> 1;
  cNodeCapPlugin1->capCNodePtr = memblock_alloc_kernel(PGSIZE, PGSIZE) >> 1;



  invoke_untyped_retype(cteRoot, 0,
    endpointTest, osEndpointObject, 0,
    lookup_slot(threadPlugin0, 0).slot, 0, 1, 0);
  invoke_untyped_retype(cteRoot, 0,
    endpointTest, osEndpointObject, 0,
    lookup_slot(threadPlugin0, 0).slot, 0, 1, 0);
}

int main(void) {
  vm_init();
  memblock_init();
  lapic_init();
  ioapic_init();
  console_init();

  char*** pluginArgv;
  load_plugin(pluginArgv);

  ipc_test();

  panic("ok");
}
