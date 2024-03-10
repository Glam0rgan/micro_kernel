#pragma once

#include <os/macros.h>
#include <os/config.h>


#ifndef __ASSEMBLER__

enum Os_MsgLimits {
    Os_MsgLengthBits = 7,
    Os_MsgExtraCapBits = 2
};

enum {
    Os_MsgMaxLength = 120,
};


#endif

#define LIB_BIT(n)  (1ul<<(n))
#define Os_MsgMaxExtraCaps (LIB_BIT(Os_MsgExtraCapBits)-1)
