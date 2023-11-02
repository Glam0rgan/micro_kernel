#include "ctypes.h"

// capability pointer
typedef u64 Cptr;


struct ExtraCaps {
    CtePtr excaprefs[os_MsgMaxExtraCaps];
};