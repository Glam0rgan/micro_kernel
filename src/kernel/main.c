#include <types.h>
#include <kernel/defs.h>
#include <kernel/mmu.h>
#include <kernel/memlayout.h>
#include <kernel/proc.h>
#include <object/objecttype.h>
#include <kernel/cspace.h>
#include <object/structures.h>
#include <model/statedata.h>
#include <kernel/thread.h>

extern Tcb* pluginTcb[5];
extern Tcb* ksCurThread;

void ipc_test(void) {

  // Alloc the cte and tcb memory.
  uint64_t* tcbObject0 = memblock_alloc_kernel(PGSIZE, PGSIZE);
  //cprintf("%l\n", *tcbObject0);
  uint64_t* tcbObject1 = memblock_alloc_kernel(PGSIZE, PGSIZE);
  //cprintf("%l\n", *tcbObject1);
  Tcb* tcbPlugin0 = (Tcb*)((char*)tcbObject0 + BIT(TCB_SIZE_BITS));

  pluginTcb[0] = tcbPlugin0;

  Tcb* tcbPlugin1 = (Tcb*)((char*)tcbObject1 + BIT(TCB_SIZE_BITS));
  pluginTcb[1] = tcbPlugin1;
  //cprintf("tcb %l\n", tcbPlugin0);

  //cprintf("tcb %l\n", tcbPlugin1);

  // Alloc tf and context.
  //tcbPlugin0->tf = memblock_alloc_kernel(PGSIZE,PGSIZE);
  //tcbPlugin0->context = memblock_alloc_kernel(PGSIZE,PGSIZE);

  //tcbPlugin1->tf = memblock_alloc_kernel(PGSIZE,PGSIZE);
  //tcbPlugin1->context = memblock_alloc_kernel(PGSIZE,PGSIZE);

  //void* cnodePtrPlugin0 = memblock_alloc_kernel(PGSIZE, PGSIZE);
  //void* cnodePtrPlugin1 = memblock_alloc_kernel(PGSIZE, PGSIZE);
  //cprintf("%l\n", cnodePtrPlugin0);
  //cprintf("%l\n", cnodePtrPlugin1);

  // Add to scheduler.
  tcb_sched_append(tcbPlugin0);
  tcb_sched_append(tcbPlugin1);
  //cprintf("next %l\n", ksReadyQueues.head);
  //cprintf("next %l\n", ksReadyQueues.end);

  // Use to create cdt.
  Cte* cteRoot = memblock_alloc_kernel(PGSIZE, PGSIZE);

  // Alloc the memory for storing cap.
  //((CNodeCap*)TCB_PTR_CTE_PTR(tcbPlugin0, tcbCTable))->capCNodePtr =            
  //  memblock_alloc_kernel(PGSIZE,PGSIZE)>>1;

  //((CNodeCap*)TCB_PTR_CTE_PTR(tcbPlugin1, tcbCTable))->capCNodePtr =     		
  //  memblock_alloc_kernel(PGSIZE,PGSIZE)>>1;

  // Init the cteRoot.
  UntypedCap untypedCap = *(UntypedCap*)(&cteRoot->cap);
  untypedCap.capPtr = memblock_alloc_kernel(PGSIZE, PGSIZE);
  cteRoot->cap = *(Cap*)(&untypedCap);
  cteRoot->cteMdbNode.mdbPrev = NULL;
  cteRoot->cteMdbNode.mdbNext = NULL;


  // Init the CNodeCap.

  Cte* cTable0 = (Cte*)tcbObject0;
  Cte* cTable1 = (Cte*)tcbObject1;

  cTable0->cap = create_object(osCapTableObject,
    memblock_alloc_kernel(PGSIZE, PGSIZE) >> 1, 0, 0);

  cTable1->cap = create_object(osCapTableObject,
    memblock_alloc_kernel(PGSIZE, PGSIZE) >> 1, 0, 0);

  //panic("asd");

  //create_new_objects(osCapTableObject, cteRoot,
  //  ((CNodeCap*)TCB_PTR_CTE_PTR(tcbPlugin1, tcbCTable))->capCNodePtr<<1
  //  | 0xFFFFF00000000000
  //  , 0, 1, cnodePtrPlugin1, 0, 0);

  // Allocate caps by root_task (need capdl).
  // Create endpoint and endpointcap.
  Endpoint* endpointTest = memblock_alloc_kernel(PGSIZE, PGSIZE);
  endpointTest->state = EPState_Send;
  //cprintf("endpoint %l\n", endpointTest);
  //panic("asd");
  Cap capTest = create_object(osEndpointObject, endpointTest, 0ul, false);

  //cprintf("%l\n", lookup_slot(tcbPlugin0,0).slot);
  //panic("asd");
  // Insert cap to the cspace.
  insert_new_cap(cteRoot,
    lookup_slot(tcbPlugin0, 0).slot,
    capTest);
  //panic("ipc_test");
  insert_new_cap(cteRoot,
    lookup_slot(tcbPlugin1, 0).slot,
    capTest);

}

int main(void) {


  // Init kernel.
  memblock_init();

  lapic_init();
  seg_init();
  //panic("seg");
  ioapic_init();

  //console_init();

  vm_init();
  //panic("vm init");

  ksReadyQueues.head = NULL;
  ksReadyQueues.end = NULL;

  ipc_test();

  char*** pluginArgv;
  load_plugin(pluginArgv);
  panic("ok7");

  //panic("pause allock");
  ksCurThread = memblock_alloc_kernel(PGSIZE, PGSIZE);
  ksCurThread->context = memblock_alloc_kernel(PGSIZE, PGSIZE);

  for(;;) {
    ksSchedulerAction = SchedulerAction_ChooseNewThread;
    schedule();
  }
  panic("ok");
}
