#include "ctypes.h"

// capability pointer
typedef u64 cptr_t;


struct extra_caps{
    cte_ptr_t excaprefs[os_MsgMaxExtraCaps];
};
typedef struct extra_caps extra_caps_t;