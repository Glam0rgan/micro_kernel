#include "types.h"
#include "defs.h"
#include "traps.h"
#include "memlayout.h"

#define IOAPIC  0xFEC00000   // Default physical address of IO APIC

#define REG_ID     0x00  // Register index: ID
#define REG_VER    0x01  // Register index: version
#define REG_TABLE  0x10  // Redirection table base

// The redirection table starts at REG_TABLE and uses
// two registers to configure each interrupt.  
// The first (low) register in a pair contains configuration bits.
// The second (high) register contains a bitmask telling which
// CPUs can serve that interrupt.
#define INT_DISABLED   0x00010000  // Interrupt disabled
#define INT_LEVEL      0x00008000  // Level-triggered (vs edge-)
#define INT_ACTIVELOW  0x00002000  // Active low (vs high)
#define INT_LOGICAL    0x00000800  // Destination is CPU id (vs APIC ID)

volatile struct Ioapic* ioapic;

// IO APIC MMIO structure: write reg, then read or write data.
struct Ioapic {
    u64 reg;
    u64 pad[3];
    u64 data;
};

static u64 ioapic_read(int reg) {
    ioapic->reg = reg;
    return ioapic->data;
}

static void ioapic_write(int reg, u64 data) {
    ioapic->reg = reg;
    ioapic->data = data;
}

void ioapic_init(void) {
    int i, id, maxintr;

    if (!ismp)
        return;

    ioapic = (volatile struct ioapic*)IO2V(IOAPIC);
    maxintr = (ioapicread(REG_VER) >> 16) & 0xFF;
    id = ioapicread(REG_ID) >> 24;
    if (id != ioapicid)
        cprintf("ioapicinit: id isn't equal to ioapicid; not a MP\n");

    // Mark all interrupts edge-triggered, active high, disabled,
    // and not routed to any CPUs.
    for (i = 0; i <= maxintr; i++) {
        ioapic_write(REG_TABLE + 2 * i, INT_DISABLED | (T_IRQ0 + i));
        ioapic_write(REG_TABLE + 2 * i + 1, 0);
    }
}

void ioapic_enable(int irq, int cpunum) {
    if (!ismp)
        return;

    // Mark interrupt edge-triggered, active high,
    // enabled, and routed to the given cpunum,
    // which happens to be that cpu's APIC ID.
    ioapic_write(REG_TABLE + 2 * irq, T_IRQ0 + irq);
    ioapic_write(REG_TABLE + 2 * irq + 1, cpunum << 24);
}
