#ifdef ENABLE_SMP_SUPPORT

typedef struct smpStatedata{
  archNodeState_t cpu;
  nodeState_t system;
} smpStatedata_t;

extern smpStatedata_t ksSMP[CONFIG_MAX_NUM_NODES];

#endif