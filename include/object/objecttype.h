#pragma once

#include <types.h>
#include <api/failures.h>
#include <object/cnode.h>
#include <object/structures.h>
#include <object/cap.h>
#include <arch/object/structures.h>
#include <object/interrupt.h>

Cap create_object(u64 objectType, void* regionBase, u64 userSize, bool deviceMemory);
void create_new_objects(u64 objectType, Cte* parent,
    Cte* destCNode, u64 destOffset, u64 destLength,
    void* regionBase, u64 userSize, bool deviceMemory);
