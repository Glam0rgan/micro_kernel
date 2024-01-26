#include <types.h>
#include <kernel/acpi.h>
#include <kernel/defs.h>
#include <kernel/memlayout.h>
#include <kernel/proc.h>

struct cpu cpus[NCPU];
extern int ismp;
int ncpu;
extern u8 ioapicid;

static struct acpi_rsdp* scan_rsdp(u32 base, u32 len) {
  u8* p;
  for(p = p2v(base); len >= sizeof(struct acpi_rsdp); len -= 4, p += 4) {
    if(memcmp(p, SIG_RSDP, 8) == 0) {
      u32 sum, n;
      for(sum = 0, n = 0; n < 20; n++)
        sum += p[n];
      if((sum & 0xff) == 0)
        return (struct acpi_rdsp*)p;
    }
  }
  return (struct acpi_rdsp*)0;
}

static struct acpi_rsdp* find_rsdp(void) {
  struct acpi_rsdp* rsdp;
  u64 pa;
  pa = *((u16*)P2V(0x40E)) << 4; // EBDA
  if(pa && (rsdp = scan_rsdp(pa, 1024)))
    return rsdp;
  return scan_rsdp(0xE0000, 0x20000);
}

static int acpi_config_smp(struct acpi_madt* madt) {
  u32 lapic_addr;
  u32 nioapic = 0;
  u8* p, * e;

  if(!madt)
    return -1;
  if(madt->header.length < sizeof(struct acpi_madt))
    return -1;

  lapic_addr = madt->lapic_addr_phys;

  p = madt->table;
  e = p + madt->header.length - sizeof(struct acpi_madt);

  while(p < e) {
    u32 len;
    if((e - p) < 2)
      break;
    len = p[1];
    if((e - p) < len)
      break;
    switch(p[0]) {
    case TYPE_LAPIC: {
      struct madt_lapic* lapic = (void*)p;
      if(len < sizeof(*lapic))
        break;
      if(!(lapic->flags & APIC_LAPIC_ENABLED))
        break;
      cprintf("acpi: cpu#%d apicid %d\n", ncpu, lapic->apic_id);
      cpus[ncpu].id = ncpu;
      cpus[ncpu].apicid = lapic->apic_id;
      ncpu++;
      break;
    }
    case TYPE_IOAPIC: {
      struct madt_ioapic* ioapic = (void*)p;
      if(len < sizeof(*ioapic))
        break;
      cprintf("acpi: ioapic#%d @%x id=%d base=%d\n",
        nioapic, ioapic->addr, ioapic->id, ioapic->interrupt_base);
      if(nioapic) {
        cprintf("warning: multiple ioapics are not supported");
      } else {
        ioapicid = ioapic->id;
      }
      nioapic++;
      break;
    }
    }
    p += len;
  }

  if(ncpu) {
    ismp = 1;
    lapic = IO2V(((u64)lapic_addr));
    return 0;
  }

  return -1;
}

#define PHYSLIMIT 0x80000000

int acpi_init(void) {
  unsigned n, count;
  struct acpi_rsdp* rsdp;
  struct acpi_rsdt* rsdt;
  struct acpi_madt* madt = 0;

  rsdp = find_rsdp();
  if(rsdp->rsdt_addr_phys > PHYSLIMIT)
    goto notmapped;
  rsdt = p2v(rsdp->rsdt_addr_phys);
  count = (rsdt->header.length - sizeof(*rsdt)) / 4;
  for(n = 0; n < count; n++) {
    struct acpi_desc_header* hdr = p2v(rsdt->entry[n]);
    if(rsdt->entry[n] > PHYSLIMIT)
      goto notmapped;
    if(!memcmp(hdr->signature, SIG_MADT, 4))
      madt = (void*)hdr;
  }

  return acpi_config_smp(madt);

notmapped:
  cprintf("acpi: tables above 0x%x not mapped.\n", PHYSLIMIT);
  return -1;
}
