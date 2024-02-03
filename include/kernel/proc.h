#pragma once
// Segments in proc->gdt.
#define NSEGS     7
#define NCPU      8
// Per-CPU state
struct cpu {
  uint8_t id;                    // index into cpus[] below
  uint8_t apicid;                // Local APIC ID
  struct context *schedulerContext;   // swtch() here to enter scheduler
  //struct taskstate ts;         // Used by x86 to find stack for interrupt
  //struct segdesc gdt[NSEGS];   // x86 global descriptor table
  //volatile uint started;       // Has the CPU started?
  //int ncli;                    // Depth of pushcli nesting.
  //int intena;                  // Were interrupts enabled before pushcli?

  // Cpu-local storage variables; see below
  void *local;
};

extern struct cpu cpus[NCPU];
extern int ncpu;

// Per-CPU variables, holding pointers to the
// current cpu and to the current process.
// The asm suffix tells gcc to use "%gs:0" to refer to cpu
// and "%gs:4" to refer to proc.  seginit sets up the
// %gs segment register so that %gs refers to the memory
// holding those two variables in the local cpu's struct cpu.
// This is similar to how thread-local variables are implemented
// in thread libraries such as Linux pthreads.
extern __thread struct cpu *cpu;

//PAGEBREAK: 17
// Saved registers for kernel context switches.
// Don't need to save all the segment registers (%cs, etc),
// because they are constant across kernel contexts.
// Don't need to save %eax, %ecx, %edx, because the
// x86 convention is that the caller has saved them.
// Contexts are stored at the bottom of the stack they
// describe; the stack pointer is the address of the context.
// The layout of the context matches the layout of the stack in swtch.S
// at the "Switch stacks" comment. Switch doesn't save eip explicitly,
// but it is on the stack and allocproc() manipulates it.
typedef struct context {
  u64 r15;
  u64 r14;
  u64 r13;
  u64 r12;
  u64 r11;
  u64 rbx;
  u64 rbp; //rbp
  u64 rip; //rip;
}Context;

// Process memory is laid out contiguously, low addresses first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap
