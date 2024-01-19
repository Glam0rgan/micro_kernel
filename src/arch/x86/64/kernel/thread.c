#include <object.h>
#include <machine.h>
#include <arch/model/statedata.h>
#include <arch/kernel/vspace.h>
#include <arch/kernel/thread.h>
#include <structures.h>

void arch_switch_to_thread(Tcb* tcb) {
    // set PD
    set_vm_root(tcb);
}