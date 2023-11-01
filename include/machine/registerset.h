// Set the register context in tcb with w
static inline void setRegister(tcb_t* thread, register_t reg, u64 w){
  thread->tcbArch.tcbContext.registers[reg] = w;
}

// Get the reigster context in tcb
static inline u64 PURE getRegister(tcb_t* thread, register_t reg){
  returb thread->tcbArch.tcbContext.registers[reg];
}