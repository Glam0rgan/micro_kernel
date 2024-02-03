// A virtual address 'va' has a five-part structute as follow
// +--------10------+--------10------+--------10------+-------10-------+---------12----------+
// |    Page-Map    | Page Directory | Page Directory |   Page Table   | Offset within Page  |
// |  Level-4 Index | Pointer Index  |      Index     |      Index     |                     |
// +----------------+----------------+----------------+----------------+---------------------+
//  \--- PML4X(va) --/ \--- PDPTX(va) --/ \--- PDX(va) --/ \--- PTX(va) --/ 

// Page Directory Pointer Index
#define PML4X(va)         (((u64)(va) >> PML4XSHIFT ) & PXMASK)
#define PDPTX(va)         (((u64)(va) >> PDPTXSHIFT ) & PXMASK)
#define PDX(va)           (((u64)(va) >> PDXSHIFT ) & PXMASK)
#define PTX(va)           (((u64)(va) >> PTXSHIFT ) & PXMASK)

// Page directory and page table constants.
#define PGSIZE          4096     // bytes a page

#define PTXSHIFT        12       // offset of PTX in a linear address
#define PDXSHIFT        21       // offset of PDX in a linear address
#define PDPTXSHIFT      30       // offset of PDPX in a linear address
#define PML4XSHIFT      39       // offset of PML4 in a linear address

#define PXMASK          0x1FF
#define OFFSET(va)      ((u64)(va) & 0xFFF)

#define PGROUNDUP(sz)  (((sz)+((u64)PGSIZE-1)) & ~((u64)(PGSIZE-1)))
#define PGROUNDDOWN(a) (((a)) & ~((u64)(PGSIZE-1)))

#define ALIGN(x,a)       (((x) + (a) -1) & ~(a-1))
#define ALIGN_DOWN(x,a)  ((x) & ~(a-1))

// Page entry flags
#define PTE_P           0x001   // Present
#define PTE_W           0x002   // Read/write
#define PTE_U           0x004   // User/supervisor
#define PTE_PWT         0x008   // Write-Through
#define PTE_PCD         0x010   // Cache-Disable
#define PTE_A           0x020   // Accessed
#define PTE_D           0x040   // Dirty
#define PTE_PS          0x080   // Page Size
#define PTE_MBZ         0x180   // Bits must be zero

// Eflags register
#define FL_CF           0x00000001      // Carry Flag
#define FL_PF           0x00000004      // Parity Flag
#define FL_AF           0x00000010      // Auxiliary carry Flag
#define FL_ZF           0x00000040      // Zero Flag
#define FL_SF           0x00000080      // Sign Flag
#define FL_TF           0x00000100      // Trap Flag
#define FL_IF           0x00000200      // Interrupt Enable
#define FL_DF           0x00000400      // Direction Flag
#define FL_OF           0x00000800      // Overflow Flag
#define FL_IOPL_MASK    0x00003000      // I/O Privilege Level bitmask
#define FL_IOPL_0       0x00000000      //   IOPL == 0
#define FL_IOPL_1       0x00001000      //   IOPL == 1
#define FL_IOPL_2       0x00002000      //   IOPL == 2
#define FL_IOPL_3       0x00003000      //   IOPL == 3
#define FL_NT           0x00004000      // Nested Task
#define FL_RF           0x00010000      // Resume Flag
#define FL_VM           0x00020000      // Virtual 8086 mode
#define FL_AC           0x00040000      // Alignment Check
#define FL_VIF          0x00080000      // Virtual Interrupt Flag
#define FL_VIP          0x00100000      // Virtual Interrupt Pending
#define FL_ID           0x00200000      // ID flag

// Control Register flags
#define CR0_PE          0x00000001      // Protection Enable
#define CR0_MP          0x00000002      // Monitor coProcessor
#define CR0_EM          0x00000004      // Emulation
#define CR0_TS          0x00000008      // Task Switched
#define CR0_ET          0x00000010      // Extension Type
#define CR0_NE          0x00000020      // Numeric Errror
#define CR0_WP          0x00010000      // Write Protect
#define CR0_AM          0x00040000      // Alignment Mask
#define CR0_NW          0x20000000      // Not Writethrough
#define CR0_CD          0x40000000      // Cache Disable
#define CR0_PG          0x80000000      // Paging

#define CR4_PSE         0x00000010      // Page size extension

#define SEG_KCODE 1  // kernel code
#define SEG_KDATA 2  // kernel data+stack
#define SEG_KCPU  3  // kernel per-cpu data
#define SEG_UCODE 4  // user code
#define SEG_UDATA 5  // user data+stack
#define SEG_TSS   6  // this process's task state

// Address in page table or page directory entry
#define PTE_ADDR(pte)   ((uint64_t)(pte) & ~0xFFF)

#define DPL_USER    0x3     // User DPL

#define BUUDY_TYPES 12
#define NULL ((void* )0)

#ifndef __ASSEMBLER__

struct MEMORY_E820{
  int nr_map;
  struct{
    u64 addr;
    u64 len;
    u32 type;
  }__attribute__((packed)) map[32];
};

// Segment Descriptor
struct segdesc {
  u32 lim_15_0 : 16;  // Low bits of segment limit
  u32 base_15_0 : 16; // Low bits of segment base address
  u32 base_23_16 : 8; // Middle bits of segment base address
  u32 type : 4;       // Segment type (see STS_ constants)
  u32 s : 1;          // 0 = system, 1 = application
  u32 dpl : 2;        // Descriptor Privilege Level
  u32 p : 1;          // Present
  u32 lim_19_16 : 4;  // High bits of segment limit
  u32 avl : 1;        // Unused (available for software use)
  u32 rsv1 : 1;       // Reserved
  u32 db : 1;         // 0 = 16-bit segment, 1 = 32-bit segment
  u32 g : 1;          // Granularity: limit scaled by 4K when set
  u32 base_31_24 : 8; // High bits of segment base address
};

// Normal segment
#define SEG(type, base, lim, dpl) (struct segdesc)    \
{ ((lim) >> 12) & 0xffff, (u64)(base) & 0xffff,      \
  ((u64p)(base) >> 16) & 0xff, type, 1, dpl, 1,       \
  (u64p)(lim) >> 28, 0, 0, 1, 1, (u64p)(base) >> 24 }
#define SEG16(type, base, lim, dpl) (struct segdesc)  \
{ (lim) & 0xffff, (u64p)(base) & 0xffff,              \
  ((u64p)(base) >> 16) & 0xff, type, 1, dpl, 1,       \
  (u64p)(lim) >> 16, 0, 0, 1, 0, (u64p)(base) >> 24 }

struct list_head{
  struct list_head *next, *prev;
};

struct free_area{
  //struct list_head free_list;
  u64 nr_free;
};

//struct zone{
//  struct free_area free_area[BUUDY_TYPES];

//}zone1;

struct page{
  //struct list_head lru;
  u64 vaddr;
  u64 paddr;
}__attribute__((packed));

#endif
