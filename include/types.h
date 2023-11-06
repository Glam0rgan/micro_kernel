#include "ctypes.h"

// capability pointer
typedef u64 Cptr;

typedef Cte* CtePtr;
// Use to save slot.
struct _ExtraCaps {
    CtePtr excaprefs[os_MsgMaxExtraCaps];
};
typedef struct _ExtraCaps ExtraCaps;