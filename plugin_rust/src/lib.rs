#![feature(const_mut_refs)]
#![feature(alloc_error_handler)]
#[alloc_error_handler]
fn alloc_error_handler(layout: alloc::alloc::Layout) -> ! {
    panic!("allocation error: {:?}", layout)
}

#[global_allocator]
static ALLOCATOR: allocator::Allocator = allocator::Allocator;