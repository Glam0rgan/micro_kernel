#ifdef ENABLE_SMP_SUPPORT
#define NODE_STATE_ON_CORE(_state, _core)  ksSMP[(_core)].system._state

#define SMP_COND_STATEMENT(_st)            _st

#else
#define NODE_STATE_ON_CORE(_state, _core)  _state

#define SMP_COND_STATEMENT(_st)

#endif

#define SchedulerAction_ResumeCurrentThread ((Tcb*)0)
#define SchedulerAction_ChooseNewThread ((Tcb*) 1)
#define NODE_STATE(_state)    NODE_STATE_ON_CORE(_state, get_current_cpu_index())