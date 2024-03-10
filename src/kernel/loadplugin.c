#include <types.h>
#include <elf.h>
#include <param.h>
#include <x86.h>
#include <defs.h>
#include <kernel/mmu.h>
#include <kernel/memlayout.h>
#include <proc.h>
#include <object/structures.h>

u32 pluginSector;
Tcb* pluginTcb[5];

#define pluginStartSection 0
#define pluginNum 2
#define SECTSIZE 512
#define MAXPLUGINS 25

void load_plugin_init() {
    pluginSector = 0;
}

void wait_disk(void) {
    // Wait for disk ready.
    while((inb(0x1f7) & 0xC0) != 0x40)
        ;
}

// Read a single sector at offset into dst.
void read_sect(void* dst, u32 offset) {
    // Issue command.
    wait_disk();
    outb(0x1f2, 1);   // count = 1
    outb(0x1f3, offset);
    outb(0x1f4, offset >> 8);
    outb(0x1f5, offset >> 16);
    outb(0x1f6, (offset >> 24) | 0xF0);
    outb(0x1f7, 0x20);  // cmd 0x20 - read sectors

    // Read data.
    wait_disk();
    insl(0x1f0, dst, SECTSIZE / 4);
}

// Read 'count' bytes at 'offset' from kernel into physical address 'pa'.
// Might copy more than asked.
void
read_seg(u8* pa, u32 count, u32 offset) {
    u8* epa;

    epa = pa + count;

    // Round down to sector boundary.
    pa -= offset % SECTSIZE;

    // Translate from bytes to sectors; kernel starts at sector 1.
    offset = (offset / SECTSIZE) + 1;

    // If this is too slow, we could read lots of sectors at a time.
    // We'd write more to memory than asked, but it doesn't matter --
    // we load in increasing order.
    for(; pa < epa; pa += SECTSIZE, offset++)
        read_sect(pa, offset);
}

int load_plugin(char*** argv) {

    load_plugin_init();
    ElfHdr elf;
    // ??? 3+MAXARG+1
    u64 argc, uStack[3 + MAXARG + 1];
    u32* pluginElf = memblock_alloc_kernel(PGSIZE*16,PGSIZE);
    struct ProgHdr progHdr;
    int cnt, off;
    u64 size, stackPoint;

    
    for(int i = 0;i < pluginNum;++i) {
    	Tcb* temp = pluginTcb[i];
    	alloc_proc(temp);
    	
        size = 4096;
        int blockSize =1;

        // Get ElfHdr
        read_seg((u8*)pluginElf, 512, pluginSector*512);
        
        elf = *(ElfHdr*)(pluginElf);
        //cprintf("elf %x\n", elf.entry);
        while(elf.magic!=ELF_MAGIC){
          pluginSector++;
          read_seg((u8*)pluginElf, 512, pluginSector*512);
        	
          elf = *(ElfHdr*)(pluginElf);	
        }
        pluginSector++;
        // Read the rest.
        ElfHdr elfTmp;
        read_seg( ((u8*)pluginElf)+ blockSize*512, 512, pluginSector*512);
        elfTmp = *(ElfHdr*)(pluginElf + blockSize*128);
        while(elfTmp.magic!=ELF_MAGIC){
          pluginSector++;
          blockSize++;
          read_seg((u8*)pluginElf+ blockSize*512, 512, pluginSector*512);
        	
          elfTmp = *(ElfHdr*)(pluginElf +blockSize*128);	
        }
        
        // setup kvm
        u64* pml4 = setup_user_memory_pages();
        //cprintf("pml4 %d\n", pluginSector);
        // Load program into memory.
        
        for(cnt = 0, off = elf.phoff; cnt <elf.phnum;cnt++, off += sizeof(progHdr)) {
            progHdr = *(struct ProgHdr*)(pluginElf + off / 4);
             
            if(progHdr.type != ELF_PROG_LOAD)
                continue;
               
            //u32* ttmp = pluginElf + progHdr.off/4;
            
            
            //for(int e=0;e<10;++e){
            //  cprintf("elf %l\n", *((uint64_t*)pluginElf + e + progHdr.off/8) );
            //}
 
            size = alloc_uvm(pml4, size, progHdr.vaddr + progHdr.memsz);
	  
            // Copy the code to user memory.
            copy_uvm(pml4, progHdr.vaddr, 
            (uint8_t*)pluginElf + progHdr.off, progHdr.memsz);
		
        }
        //panic("load");
        size = PGROUNDUP(size);
        size = alloc_uvm(pml4, size, size + 2 * PGSIZE);
        
        //clear_pteu(pml4, size - 2 * PGSIZE);
        
        stackPoint = size;

	/*
        // Push argument strings, prepare rest of stack in userstack.
        for(argc = 0;argv[cnt][argc];argc++) {
            stackPoint = (stackPoint - (strlen(argv[cnt][argc]) + 1)) & ~(sizeof(u64) - 1);
            copy_uvm(pml4, stackPoint, argv[cnt][argc], strlen(argv[cnt][argc] + 1));
            uStack[3 + argc] = stackPoint;
        }
        */
        argc = 0;
        uStack[3 + argc] = 0;

        uStack[0] = 0xffffffff; // fake return pc
        uStack[1] = argc;
        uStack[2] = stackPoint - (argc + 1) * sizeof(u64); //argv pointer

        temp->tf->rdi = argc;
        temp->tf->rsi = stackPoint - (argc + 1) * sizeof(u64);

        stackPoint -= (3 + argc + 1) * sizeof(u64);
        //cprintf(" %l %l %l\n", stackPoint, uStack, (3 + argc +1)*sizeof(u64));
        //panic("load_p");        
        copy_uvm(pml4, stackPoint, uStack, (3 + argc + 1) * sizeof(u64));
        //panic("load_plugin");
	temp->pml4 = pml4;
        temp->size = size;
        temp->tf->rip = elf.entry;
        //cprintf("tf->rip %l\n", elf.entry);
        //panic("cc");
        temp->tf->rsp = stackPoint;
        
        temp->tf->cs = (SEG_UCODE << 3) | DPL_USER ;
        
        temp->tf->ds = (SEG_UDATA << 3) | DPL_USER;
        //temp->tf->ds = 0;
        temp->tf->eflags = 0;
        //panic("load");
        temp->tcbIPCBuffer = p2v(page_walk_for_others(pml4,0x2000,0));
        
    }
}
