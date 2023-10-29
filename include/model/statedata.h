#ifdef ENABLE_SMP_SUPPORT
#define NODE_STATE_ON_CORE(_state, _core)  ksSMP[(_core)].system._state
#else
#define NODE_STATE_ON_CORE(_state, _core)  _state
#endif
#define NODE_STATE(_state)    NODE_STATE_ON_CORE(_state, get_current_cpu_index())