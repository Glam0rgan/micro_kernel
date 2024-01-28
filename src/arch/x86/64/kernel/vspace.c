#include <config.h>
#include <api/failures.h>
#include <machine/io.h>
#include <object/structures.h>
#include <arch/kernel/vspace.h>
#include <mode/machine.h>
#include <machine.h>

// Change the cr3 register
void set_vm_root(Tcb* tcb) {
    Cap threadRoot;
    Pml4e* pml4;
    FindVSpaceForASIDRet findRet;
    Cr3 cr3;

    threadRoot = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    Pml4Cap pml4Cap = *(Pml4Cap*)(&threadRoot);
    pml4 = PML4E_PTR(pml4Cap.capPML4BasePtr);
    cr3 = make_cr3(pptr_to_paddr(pml4));
    set_current_user_cr3(cr3);
}
