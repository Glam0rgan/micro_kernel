
#define CONFIG_XSAVE_SIZE 0
#define N_CONTEXT_REGISTERS 0
// x86 fpu context
struct UserFpuState {
  u8 state[CONFIG_XSAVE_SIZE];
};

// x86 user-code context
struct UserContext {
  UserFpuState fpuState;
  u64 registers[N_CONTEXT_REGISTERS];
#if defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_IA32)
  // stored pointer to kernel stack used when kernel run in current TCB context.
  u64 kernelSp;
#endif
};