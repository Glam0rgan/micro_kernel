typedef struct arch_tcb{
  user_context_t tcb_context;
#ifdef CONFIG_VTX
  struct vcpu * tcbVCPU;
#endif
} arch_tcb_t;