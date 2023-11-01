enum vm_page_size {
  X86_SmallPage,
  X86_LargePage,
  X64_HugePage
};
typedef u64 vm_page_size_t;

// Get the page bits correspond to the page size type
static inline u64 CONST pageBitsForSize(vm_page_size_t pagesize) {
  switch (pagesize) {
  case X86_SmallPage:
    return os_PageBits;

  case X86_LargePage:
    return os_LargePageBits;

  case X64_HugePage:
    return os_HugePageBits;

  default:
    panic("Invalid page size");
  }
}