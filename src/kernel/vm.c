#include <types.h>
#include <kernel/defs.h>
#include <kernel/mmu.h>
#include <kernel/memlayout.h>
#include <kernel/vm.h>
#include <object/tcb.h>
#include <kernel/x86.h>
#include <object/structures.h>


#define kernelFirstSize 0x40000000
#define KernelFirstHasMappedEnd  0xFFFFFFFFA0000000

u64 kernelHasMappedEnd;
u64 kernelSize;

__thread struct cpu* cpu;

static u64* kernPml4;
static u64* kernPdpt;
static u64* kernPd;
static u64* kernPt;

static u64* page_walk(u64* pml4e, const void* va, int alloc);

static void mkgate(u32* idt, u32 n, void* kva, u32 pl, u32 trap) {
  uint64_t addr = (uint64_t)kva;
  n *= 4;
  trap = trap ? 0x8F00 : 0x8E00; // TRAP vs INTERRUPT gate;
  idt[n + 0] = (addr & 0xFFFF) | ((SEG_KCODE << 3) << 16);
  idt[n + 1] = (addr & 0xFFFF0000) | trap | ((pl & 3) << 13); // P=1 DPL=pl
  idt[n + 2] = addr >> 32;
  idt[n + 3] = 0;
}

void wrmsr(u32 msr, uint64_t val);

extern void* vectors[];

// Set up CPU's kernel segment descriptors.
// Run once on entry on each CPU.
void seg_init(void) {
  uint64_t* gdt;
  u32* tss;
  uint64_t addr;
  void* local;
  struct cpu* c;
  u32* idt = (u32*)memblock_alloc_kernel(PGSIZE * 8, PGSIZE);
  int n;
  memset(idt, 0, PGSIZE * 4);

  for(n = 0; n < 256; n++)
    mkgate(idt, n, vectors[n], 0, 0);
  mkgate(idt, 64, vectors[64], 3, 1);

  lidt((void*)idt, PGSIZE);

  // create a page for cpu local storage 
  local = memblock_alloc_kernel(PGSIZE, PGSIZE);
  memset(local, 0, PGSIZE);

  gdt = (uint64_t*)local;
  tss = (u32*)(((char*)local) + 1024);
  tss[16] = 0x00680000; // IO Map Base = End of TSS

  // point FS smack in the middle of our local storage page
  wrmsr(0xC0000100, ((uint64_t)local) + (PGSIZE / 2));

  c = &cpus[0];
  c->local = local;

  cpu = c;

  addr = (uint64_t)tss;
  gdt[0] = 0x0000000000000000;
  gdt[SEG_KCODE] = 0x0020980000000000;  // Code, DPL=0, R/X
  gdt[SEG_UCODE] = 0x0020F80000000000;  // Code, DPL=3, R/X
  gdt[SEG_KDATA] = 0x0000920000000000;  // Data, DPL=0, W
  gdt[SEG_KCPU] = 0x0000000000000000;  // unused
  gdt[SEG_UDATA] = 0x0000F20000000000;  // Data, DPL=3, W
  gdt[SEG_TSS + 0] = (0x0067) | ((addr & 0xFFFFFF) << 16) |
    (0x00E9LL << 40) | (((addr >> 24) & 0xFF) << 56);
  gdt[SEG_TSS + 1] = (addr >> 32);

  lgdt((void*)gdt, 8 * sizeof(uint64_t));

  loadss(SEG_KDATA << 3);

  ltr(SEG_TSS << 3);
};

static void tss_set_rsp(u32* tss, u32 n, uint64_t rsp) {
  tss[n * 2 + 1] = rsp;
  tss[n * 2 + 2] = rsp >> 32;
}

void kernel_switch_vm(void) {
  lcr3(v2p(kernPml4));
  //panic("kernel_switch_vm");
}

void user_switch_vm(Tcb* thread) {

  // Change tss.
  u32* tss;

  tss = (u32*)(((char*)cpu->local) + 1024);
  tss_set_rsp(tss, 0, (u64)thread->kStack + PGSIZE);

  //panic("user_switch_vm");
  //  Change pml4.
  lcr3(v2p(thread->pml4));
  //cprintf("page walk %l\n", page_walk(kernPml4,0xFFFFFFFF9FECD000,0));
  //panic("error");
}

// Return the physical address in pml4
// that corresponds to virtual address va. If alloc!=0,
// create any required page table pages.
static u64* page_walk(u64* pml4e, const void* va, int alloc) {

  u64* pdpte, * pde, * pte;

  /*
  if(!(*pml4e & PTE_P)) {
    if(!alloc || (pml4e = (u64*)memblock_alloc_kernel(512, 512)) == 0)
      return 0;
    memset(pml4e, 0, 512);
  }
  */


  // Page directory pointer index
  pdpte = p2v(pml4e[PML4X(va)]);

  if(!((uint64_t)pdpte & PTE_P)) {
    //panic("??");

    if(!alloc || (pdpte = (u64*)memblock_alloc_kernel(PGSIZE, PGSIZE)) == 0)
      return 0;
    memset(pdpte, 0, 512);
    pml4e[PML4X(va)] = v2p(pdpte) | PTE_P | PTE_W | PTE_U;
  }
  pdpte = PTE_ADDR(pdpte);

  // Page directory index
  pde = p2v(pdpte[PDPTX(va)]);

  if(!((uint64_t)pde & PTE_P)) {
    //panic("???");
    if(!alloc || (pde = (u64*)memblock_alloc_kernel(PGSIZE, PGSIZE)) == 0)
      return 0;
    memset(pde, 0, 512);
    pdpte[PDPTX(va)] = v2p(pde) | PTE_P | PTE_W | PTE_U;
  }
  pde = PTE_ADDR(pde);


  // Page table index

  pte = p2v(pde[PDX(va)]);
  //cprintf("pte : %l\n", (uint64_t)pte);
  if(!((uint64_t)pte & PTE_P)) {
    //panic("????");
    if(!alloc || (pte = (u64*)memblock_alloc_kernel(PGSIZE, PGSIZE)) == 0)
      return 0;
    memset(pte, 0, 512);
    pde[PDX(va)] = v2p(pte) | PTE_P | PTE_W | PTE_U;
  }
  pte = PTE_ADDR(pte);

  //cprintf("va pdx : %x %d\n",va,PTX(va));

  u64* pg = (pte[PTX(va)] & 0xFFFFFFFFFFFFF000) | OFFSET(va);
  //cprintf("pg : %l\n", pg);
  return pg;
}

// Return the address of the PTE in pml4
// that corresponds to virtual address va. If alloc!=0,
// create any required page table pages.
static u64* page_walk_return_pte(u64* pml4e, const void* va, int alloc) {

  u64* pdpte, * pde, * pte;

  /*
  if(!(*pml4e & PTE_P)) {
    if(!alloc || (pml4e = (u64*)memblock_alloc_kernel(512, 512)) == 0)
      return 0;
    memset(pml4e, 0, 512);
  }
  */


  // Page directory pointer index
  pdpte = p2v(pml4e[PML4X(va)]);

  if(!((uint64_t)pdpte & PTE_P)) {

    if(!alloc || (pdpte = (u64*)memblock_alloc_kernel(PGSIZE, PGSIZE)) == 0)
      return 0;
    memset(pdpte, 0, 512);
    pml4e[PML4X(va)] = v2p(pdpte) | PTE_P | PTE_W | PTE_U;
  }
  pdpte = PTE_ADDR(pdpte);

  // Page directory index
  pde = p2v(pdpte[PDPTX(va)]);

  if(!((uint64_t)pde & PTE_P)) {

    if(!alloc || (pde = (u64*)memblock_alloc_kernel(PGSIZE, PGSIZE)) == 0)
      return 0;
    memset(pde, 0, 512);
    pdpte[PDPTX(va)] = v2p(pde) | PTE_P | PTE_W | PTE_U;
  }
  pde = PTE_ADDR(pde);


  // Page table index

  pte = p2v(pde[PDX(va)]);

  if(!((uint64_t)pte & PTE_P)) {

    if(!alloc || (pte = (u64*)memblock_alloc_kernel(PGSIZE, PGSIZE)) == 0)
      return 0;
    memset(pte, 0, 512);
    pde[PDX(va)] = v2p(pte) | PTE_P | PTE_W | PTE_U;
  }
  pte = PTE_ADDR(pte);


  return &pte[PTX(va)];
}

u64* page_walk_for_others(u64* pml4e, const void* va, int alloc) {

  u64* pdpte, * pde, * pte;

  /*
  if(!(*pml4e & PTE_P)) {
    if(!alloc || (pml4e = (u64*)memblock_alloc_kernel(512, 512)) == 0)
      return 0;
    memset(pml4e, 0, 512);
  }
  */


  // Page directory pointer index
  pdpte = p2v(pml4e[PML4X(va)]);

  if(!((uint64_t)pdpte & PTE_P)) {
    //panic("??");

    if(!alloc || (pdpte = (u64*)memblock_alloc_kernel(PGSIZE, PGSIZE)) == 0)
      return 0;
    memset(pdpte, 0, 512);
    pml4e[PML4X(va)] = v2p(pdpte) | PTE_P | PTE_W | PTE_U;
  }
  pdpte = PTE_ADDR(pdpte);

  // Page directory index
  pde = p2v(pdpte[PDPTX(va)]);

  if(!((uint64_t)pde & PTE_P)) {
    //panic("???");
    if(!alloc || (pde = (u64*)memblock_alloc_kernel(PGSIZE, PGSIZE)) == 0)
      return 0;
    memset(pde, 0, 512);
    pdpte[PDPTX(va)] = v2p(pde) | PTE_P | PTE_W | PTE_U;
  }
  pde = PTE_ADDR(pde);


  // Page table index

  pte = p2v(pde[PDX(va)]);
  //cprintf("pte : %l\n", (uint64_t)pte);
  if(!((uint64_t)pte & PTE_P)) {
    //panic("????");
    if(!alloc || (pte = (u64*)memblock_alloc_kernel(PGSIZE, PGSIZE)) == 0)
      return 0;
    memset(pte, 0, 512);
    pde[PDX(va)] = v2p(pte) | PTE_P | PTE_W | PTE_U;
  }
  pte = PTE_ADDR(pte);

  //cprintf("va pdx : %x %d\n",va,PTX(va));

  u64* pg = (pte[PTX(va)] & 0xFFFFFFFFFFFFF000) | OFFSET(va);
  //cprintf("pg : %l\n", pg);
  return pg;
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa. va and size might not
// be page-aligned.
static int map_pages(u64* pml4, void* va, u64 size, u64 pa, int perm) {
  char* first, * last;
  u64* pte;

  first = (char*)PGROUNDDOWN((u64)va);
  last = (char*)PGROUNDDOWN(((u64)va) + size - 1);

  //cprintf("first = %l\n", first);
  //cprintf("last = %l\n", last);

  while(1) {
    if((pte = page_walk_return_pte(pml4, first, 1)) == 0)
      return -1;
    //cprintf("%l\n", *pte);
    if(*pte & PTE_P)
      panic("remap");
    *pte = pa | perm | PTE_P;
    //cprintf("*pte %l\n", *pte);
    if(first == last)
      break;
    first += PGSIZE;
    pa += PGSIZE;
  }
  return 0;
}

u64* setup_user_memory_pages(void) {
  u64* pml4 = (u64*)memblock_alloc_kernel(PGSIZE, PGSIZE);
  u64* pdpt = (u64*)memblock_alloc_kernel(PGSIZE, PGSIZE);
  u64* pgdir = (u64*)memblock_alloc_kernel(PGSIZE, PGSIZE);
  u64* pte = (u64*)memblock_alloc_kernel(PGSIZE, PGSIZE);

  memset(pml4, 0, 512);
  memset(pdpt, 0, 512);
  memset(pgdir, 0, 512);
  memset(pte, 0, 512);

  pml4[0] = v2p(pdpt) | PTE_P | PTE_W | PTE_U;
  pml4[511] = kernPml4[511];
  pdpt[0] = v2p(pgdir) | PTE_P | PTE_W | PTE_U;
  pgdir[0] = v2p(pte) | PTE_P | PTE_W | PTE_U;

  *pte = memblock_alloc(PGSIZE, PGSIZE) | PTE_P | PTE_W | PTE_U;

  return pml4;
}

int alloc_uvm(u64* pml4, u64 oldSize, u64 newSize) {
  char* mem;

  if(newSize < oldSize)
    return oldSize;

  u64 oldSizeAlign = PGROUNDUP(oldSize);
  mem = memblock_alloc(newSize - oldSize, PGSIZE);
  //cprintf("mem %l\n", p2v(mem));
  // Dealloc has memset zero.
  for(;oldSizeAlign < newSize;oldSizeAlign += PGSIZE, mem += PGSIZE) {
    map_pages(pml4, (char*)oldSizeAlign, PGSIZE, mem, PTE_W | PTE_U);
  }
  return newSize;
}

int alloc_stack(u64* pml4, u64 oldSize, u64 newSize) {
  char* mem;

  if(newSize > oldSize)
    return oldSize;

  u64 newSizeAlign = PGROUNDUP(newSize);
  mem = memblock_alloc(newSize - oldSize, PGSIZE);
  // Dealloc has memset zero.
  for(;newSizeAlign < oldSize;newSizeAlign += PGSIZE, mem += PGSIZE) {
    map_pages(pml4, (char*)(newSizeAlign), PGSIZE, mem, PTE_W | PTE_U);
  }
  return newSize;
}

// Copy len bytes from p to user address va in pml4.
// Most useful when pgdir is not the current page table.
int copy_uvm(u64* pml4, u64 va, void* src, u64 len) {
  u64 vaAlign, size;
  char* buf, * ka;
  //if(len == 20){ 
    //cprintf("%l %l %l %l\n", pml4, va, (uint64_t)src, len);
    //panic("cpoy_uvm");
  //}
  buf = (char*)src;
  while(len > 0) {
    vaAlign = (u64)PGROUNDDOWN(va);

    //cprintf("va %l\n",vaAlign);
    //panic("asd");

    ka = (char*)p2v(page_walk(pml4, (void*)vaAlign, 0));
    //cprintf("%l\n", ka);
    //panic("asd");
    size = PGSIZE - (va - vaAlign);
    if(size > len)
      size = len;



    memmove(ka + (va - vaAlign), buf, size);
    //for(int i=0;i<10;i++){
    //  cprintf("%l\n", *((uint64_t*)ka + i));
    //}
    //panic("asdsad");
    len -= size;
    buf += size;
    va = vaAlign + PGSIZE;
  }

  //cprintf("%l\n", buf);


  return 0;
}

// Clear PTE_U on a page. Used to create an inaccessible
// page beneath the user stack.
void clear_pteu(u64* pml4, char* va) {
  u64* pte;

  pte = page_walk_return_pte(pml4, va, 0);
  if(pte == 0)
    panic("clear_pteu");
  *pte &= ~PTE_U;
}


// Map memory from 0xFFFFFFFF80000000
// 512MB
void vm_init(void) {
  kernelHasMappedEnd = KernelFirstHasMappedEnd;
  kernelSize = kernelFirstSize;
  // Set the new kernel page frame.
  kernPml4 = memblock_alloc_kernel(PGSIZE, PGSIZE);
  //cprintf("pml4 = %l\n",*kernPml4);
  kernPdpt = memblock_alloc_kernel(PGSIZE, PGSIZE);
  //cprintf("pdpt = %l\n",*kernPdpt);
  kernPml4[511] = v2p(kernPdpt) | PTE_W | PTE_P;
  //cprintf("511 = %l\n",kernPml4[511]);
  kernPd = memblock_alloc_kernel(PGSIZE, PGSIZE);
  //cprintf("pd = %l\n",*kernPd);
  kernPdpt[510] = v2p(kernPd) | PTE_W | PTE_P;

  for(u64 pdOffset = 0;pdOffset < 256;pdOffset++) {
    kernPt = memblock_alloc_kernel(PGSIZE, PGSIZE);
    //
    kernPd[pdOffset] = v2p(kernPt) | PTE_W | PTE_P;
    //cprintf("pd = %l\n",*kernPt);
    for(u64 ptOffset = 0;ptOffset < 512;ptOffset++) {
      kernPt[ptOffset] = (0x200000 * pdOffset + 0x1000 * ptOffset) | PTE_W | PTE_P;
      //if(pdOffset==0 && ptOffset < 10){
      //  cprintf("kernpt %l\n", kernPt[ptOffset]);
      //}
    }
  }

  //for(int i=0;i<10;++i){
  //cprintf("page walk %l\n", page_walk(kernPml4,0xFFFFFFFF9FECD000,0));
  //}
  //u64* testva = page_walk(kernPml4,0xFFFFFFFF9FECD000,0);
  //cprintf("%l\n", testva);
  //panic("vm init");
  //cprintf("vm init\n");

  kernel_switch_vm();
  //panic("bug");
}
