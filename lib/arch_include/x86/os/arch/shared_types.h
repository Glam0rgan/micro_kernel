#pragma once

enum OsFaultType {
  NullFault = 0,
  CapFault = 1,
  UnkownSyscall = 2,
  UserException = 3,
  VMFault = 4
};