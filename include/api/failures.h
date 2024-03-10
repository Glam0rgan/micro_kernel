#pragma once

#include <types.h>
#include <os/errors.h>
#include <os/constants.h>

typedef u64 Exception;
enum ExceptionType {
  EXCEPTION_NONE,
  EXCEPTION_FAULT,
  EXCEPTION_LOOKUP_FAULT,
  EXCEPTION_SYSCALL_ERROR,
  EXCEPTION_PREEMPTED
};
typedef u64 Exception;
typedef u64 SyscallErrorType;

struct _SyscallError {
  u64 invalidArgumentNumer;
  u64 invalidCapNumber;
  u64 rangeErrorMin;
  u64 rangeErrorMax;
  u64 memoryLeft;
  bool failedLookupWasSource;

  SyscallErrorType type;
};
typedef struct _SyscallError SyscallError;

extern LookupFault currentLookupFault;
extern enum OsFaultType currentFault;
extern SyscallError currentSyscallError;
