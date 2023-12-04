#include "types.h"
#include "defs.h"
#include "mmu.h"
#include "memlayout.h"
#include "proc.h"

void ipc_test(void) {
  Endpoint* endpointTest = memblock_alloc_kernel(PGSIZE, PGSIZE);

  Cte* cteRoot = memblock_alloc_kernel(PGSIZE, PGSIZE);;
  cteRoot->cteMdbNode.mdbPrev = NULL;
  cteRoot->cteMdbNode.mdbNext = NULL;

  CNodeCap* cNodeCapPlugin0 = (CNodeCap*)(&(TCB_PTR_CTE_PTR(threadPlugin0, tcbCTable)->cap));
  CNodeCap* cNodeCapPlugin1 = (CNodeCap*)(&(TCB_PTR_CTE_PTR(threadPlugin1, tcbCTable)->cap));

  cNodeCapPlugin0->capCNodePtr = memblock_alloc_kernel(PGSIZE, PGSIZE);
  cNodeCapPlugin1->capCNodePtr = memblock_alloc_kernel(PGSIZE, PGSIZE);



  invoke_untyped_retype(cteRoot, 0,
    endpointTest, osEndpointObject, 0,
    destCNode, 0, 1, 0)
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
