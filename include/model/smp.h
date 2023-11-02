#ifdef ENABLE_SMP_SUPPORT
struct SmpStateData {
  archNodeState_t cpu;
  nodeState_t system;
};

extern SmpStatedata ksSMP[CONFIG_MAX_NUM_NODES];

#endif