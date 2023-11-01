
#define CONFIG_XSAVE_SIZE 0
#define N_CONTEXT_REGISTERS 0
// x86 fpu context
struct user_fpu_state {
  u8 state[CONFIG_XSAVE_SIZE];
};
typedef struct user_fpu_state user_fpu_state_t;

// x86 user-code context
struct user_context {
  user_fpu_state_t fpu_state;
  u64 registers[N_CONTEXT_REGISTERS];
#if defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_IA32)
  // stored pointer to kernel stack used when kernel run in current TCB context.
  u64 kernel_sp;
#endif
};
typedef user_context user_context_t;