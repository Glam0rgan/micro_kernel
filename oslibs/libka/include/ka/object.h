#pragma once

#include <ka/ka.h>
#include <string.h>

typedef struct KaObject_t {
    osCPtr cptr;
    osWord ut;
    osWord type;
    osWord sizeBits;
}KaObject;

static inline int ka_alloc_object_at_maybe_dev(Ka* ka, OsWord type, OsWord sizeBits, u64 paddr, bool canUseDev
    , KaObject* result) {
    int error = -1;
    if(!(type < OSObjectTypeCount)) {
        result->cptr = 0;
        goto error;
    }

    error = ka_cspace_alloc(ka, &result->cptr);



error:
    if(result->cptr) {
        // ka_cspace_free
    }
    memset(result, 0, sizeof(*result));
    return error;
}

static inline int ka_alloc_object_at(Ka* ka, OsWord type, OsWord sizeBits, u64 paddr, KaObject* result) {
    return ka_alloc_object_at_maybe_dev(ka, type, sizeBits, paddr, false, result);
}

static inline int ka_alloc_object(Ka* ka, OsWord type, OsWord sizeBits, KaObject* result) {
    return ka_alloc_object_at(ka, type, sizeBits, KA_NO_PADDR, result);
}

static inline int ka_alloc_endpoint(Ka* ka, KaObject* result) {
    return ka_alloc_object(ka, osEndpointObject, OsEndpointBits, result);
}