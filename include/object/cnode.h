#pragma once

#include <types.h>
#include <api/failures.h>
#include <object/structures.h>

Cte* get_receive_slots(Tcb* thread, u64* buffer);
void insert_new_cap(Cte* parent, Cte* slot, Cap cap);
Cte* get_receive_slots(Tcb* thread, u64* buffer);
void cte_insert(Cap newCap, Cte* srcSlot, Cte* destSlot);