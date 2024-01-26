#pragma once

#include <config.h>
#include <hardware.h>
#include <arch/model/statedata.h>
#include <arch/machine/cpu_registers.h>
#include <arch/machine.h>
#include <mode/object/structures.h>

extern Cr3 x64KSCurrentCR3;

static inline Cr3 make_cr3(PAddr addr) {
    Cr3 cr3;
    cr3.pml4BaseAddress = addr;
    cr3.pcid = 0;
    return cr3;
}

// Address space control

static inline Cr3 get_current_cr3(void) {
    return x64KSCurrentCR3;
}

static inline Cr3 get_current_user_cr3(void) {
    return get_current_cr3();
}

static inline void set_current_cr3(Cr3 cr3) {
    x64KSCurrentCR3 = cr3;
    uint64_t cr3U64 = *(u64*)&cr3;
    write_cr3(cr3U64);
}

/* there is no option for preservation translation when setting the user cr3
   as it is assumed you want it preserved as you are doing a context switch.
   If translation needs to be flushed then setCurrentCR3 should be used instead */
static inline void set_current_user_cr3(Cr3 cr3) {
    set_current_cr3(cr3);
}
