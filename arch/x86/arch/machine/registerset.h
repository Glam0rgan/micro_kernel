// x86 fpu context
struct user_fpu_state{
  u8 state[CONFIG_XSAVE_SIZE];
};
typedef struct user_fpu_state user_fpu_state_t;

// x86 user-code context
struct user_context{
  user_fpu_state_t fpu_state;
  u64 registers[N_CONTEXT_REGISTERS];
#if defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_IA32)
  u64 kernel_sp;
#endif
}