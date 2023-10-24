#include "types.h"
typedef u64 exception_t;
enum exception{
  EXCEPTION_NONE,
  EXCEPTION_FAULT,
  EXCEPTION_LOOKUP_FAULT,
  EXCEPTION_SYSCALL_ERROR,
  EXCEPTION_PREEMPTED
};