// References: ACPI 5.0 Errata A
// http://acpi.info/spec.htm

// 5.2.5.3
#define SIG_RSDP "RSD PTR "
struct acpi_rsdp {
  u8 signature[8];
  u8 checksum;
  u8 oem_id[6];
  u8 revision;
  u32 rsdt_addr_phys;
  u32 length;
  u64 xsdt_addr_phys;
  u8 xchecksum;
  u8 reserved[3];
} __attribute__((__packed__));

// 5.2.6
struct acpi_desc_header {
  u8 signature[4];
  u32 length;
  u8 revision;
  u8 checksum;
  u8 oem_id[6];
  u8 oem_tableid[8];
  u32 oem_revision;
  u8 creator_id[4];
  u32 creator_revision;
} __attribute__((__packed__));

// 5.2.7
struct acpi_rsdt {
  struct acpi_desc_header header;
  u32 entry[0];
} __attribute__((__packed__));

#define TYPE_LAPIC 0
#define TYPE_IOAPIC 1
#define TYPE_INT_SRC_OVERRIDE 2
#define TYPE_NMI_INT_SRC 3
#define TYPE_LAPIC_NMI 4

// 5.2.12 Multiple APIC Description Table
#define SIG_MADT "APIC"
struct acpi_madt {
  struct acpi_desc_header header;
  u32 lapic_addr_phys;
  u32 flags;
  u8 table[0];
} __attribute__((__packed__));

// 5.2.12.2
#define APIC_LAPIC_ENABLED 1
struct madt_lapic {
  u8 type;
  u8 length;
  u8 acpi_id;
  u8 apic_id;
  u32 flags;
} __attribute__((__packed__));

// 5.2.12.3
struct madt_ioapic {
  u8 type;
  u8 length;
  u8 id;
  u8 reserved;
  u32 addr;
  u32 interrupt_base;
} __attribute__((__packed__));
