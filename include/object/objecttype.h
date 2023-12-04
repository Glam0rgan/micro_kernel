#pragma once

#include <types.h>
#include <api/failures.h>
#include <object/cnode.h>
#include <object/structures.h>
#include <object/cap.h>
#include <arch/object/structures.h>
#include <object/interrupt.h>

Cap create_object(u64 objectType, void* regionBase, u64 userSize, bool deviceMemory);