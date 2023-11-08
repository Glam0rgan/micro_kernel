#pragma once

#include <stdint.h>
#include <api/types.h>
#include <object/structures.h>
#include <arch/types.h>

typedef Cte* CtePtr;
// Use to save slot.
struct _ExtraCaps {
    CtePtr excaprefs[os_MsgMaxExtraCaps];
};
typedef struct _ExtraCaps ExtraCaps;