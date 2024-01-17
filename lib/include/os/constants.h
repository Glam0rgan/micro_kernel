#pragma once

#include <os/config.h>
#include <os/macros.h>

#ifndef __ASSEMBLER__

enum Os_MsgLimits {
    Os_MsgLengthBits = 7,
    Os_MsgExtraCapBits = 2
};

enum {
    Os_MsgMaxLength = 120,
}

#define Os_MsgMaxExtraCaps (LIB_BIT(Os_MsgExtraCapBits)-1)

#endif