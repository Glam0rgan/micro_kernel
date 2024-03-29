#include <types.h>
#include <object/untyped.h>

// Choose some capability in untyped element to create a new CNode.
Exception invoke_untyped_retype(Cte* srcSlot, bool reset,
    void* retypeBase, u64 newType, u64 userSize,
    Cte* destCNode, u64 destOffset, u64 destLength,
    bool deviceMemory) {

    u64 freeRef;
    u64 totalObjectSize;

    UntypedCap untypedCap = *(UntypedCap*)(&srcSlot->cap);
    void* regionBase = (u64*)(untypedCap.capPtr);

    Exception status;

    // reset

    /* Update the amount of free space left in this untyped cap.
     *
     * Note that userSize is not necessarily the true size of the object in
     * memory. In the case where newType is OsCapTableObject, the size is
     * transformed by getObjectSize. */

    totalObjectSize = destLength << get_object_size(newType, userSize);
    freeRef = (u64)retypeBase + totalObjectSize;

    // GET_FREE_INDEX(regionBase, freeRef)
    untypedCap.capFreeIndex = 0;
    srcSlot->cap = *(Cap*)(&untypedCap);

    // Create new objects and caps.
    create_new_objects(newType, srcSlot, destCNode, destOffset, destLength,
        retypeBase, userSize, deviceMemory);

    return EXCEPTION_NONE;
}
