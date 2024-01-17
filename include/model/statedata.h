#pragma once

#include <config.h>
#include <types.h>
#include <object/structures.h>
#include <object/tcb.h>
#include <mode/types.h>

#define NODE_STATE_ON_CORE(_state, _core)  _state

#define SMP_COND_STATEMENT(_st)


#define SchedulerAction_ResumeCurrentThread ((Tcb*)0)
#define SchedulerAction_ChooseNewThread ((Tcb*) 1)
#define NODE_STATE(_state)    NODE_STATE_ON_CORE(_state, get_current_cpu_index())

TcbQueue ksReadyQueues;
Tcb* ksCurThread;
