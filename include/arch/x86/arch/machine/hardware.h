#pragma once

#include <os/os_arch/constants.h>
#include <util.h>


enum VmPageSize {
  X86_SmallPage,
  X86_LargePage,
  X64_HugePage
};

//Get the page bits correspond to the page size type
static inline uint64_t CONST pageBits_forSize(enum VmPageSize pagesize) {
  switch(pagesize) {
  case X86_SmallPage:
    return OsPageBits;

  case X86_LargePage:
    return OsLargePageBits;

  case X64_HugePage:
    return OsHugePageBits;

  default:
    panic("Invalid page size");
  }
}


