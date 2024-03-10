struct Link{
    next : *mut Link,
    data : u64, 
} 

struct kernel_memory{
    freeList : *mut Link;
}

impl new_link(next : *mut Link, data : u64) -> Link{
  Link {
    next,
    data,
  }
}

mut kernel_memory memoryAllocator;

// Init the allocator, use function to get 128MB from kernel.
fn allocator_init() {

  unsafe{
    // Alloc 128MB memory from kernel
    let memoryStart : u64 = brk(32768);
    let mut index : i32 = 1;
   
    let mut memoryAddress : u64 = memoryStart;
    while(index <= 32768){
      
     if(index == 1){
        let mut memoryAllocator.freeList = &new_link(0,memoryAddress);
     }
     else{
        let mut memoryAllocator.freeList.next = &new_link(0,memoryAddress);
        let mut memoryAllocator.freeList = memoryAllocator.freeList.next;
     }
    
     index = index + 1;
     memoryAddress = memoryAddress + 0x1000;
   }

  }
  

}

// Allocator one page, get the physical address
fn physical_memory_alloc() -> u64 {
  if(memoryAllocator.freeList != 0x0){
    let resultAddress = memoryAllocator.freeList.data;
    memoryAllocator.freeList = memoryAllocator.freeList.next;
    resultAddress
  }
  else{
    0x0
  }
}

