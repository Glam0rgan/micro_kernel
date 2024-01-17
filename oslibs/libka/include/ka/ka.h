#pragma once

#include <os/os.h>
#include <os/types.h>

#define KA_NO_PADDR 1
typedef struct Ka_t {
    void* data;

}Ka;

static inline int ka_cspace_alloc(Ka* ka, OsCPtr* res) {

}