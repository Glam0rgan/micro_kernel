#pragma once

#include <os/os_arch/constants.h>

enum VmPageSize {
  X86_SmallPage,
  X86_LargePage,
  X64_HugePage
};

// Get the page bits correspond to the page size type
static inline u64 CONST pageBits_forSize(VmPageSize pagesize) {
  switch(pagesize) {
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