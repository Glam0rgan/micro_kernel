# 0 "src/kernel/entry64.S"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "src/kernel/entry64.S"
# 0 "src/kernel/entry64.S"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "src/kernel/entry64.S"
# 29 "src/kernel/entry64.S"
.code32
.global mboot_header
.global mboot_entry

mboot_header:
  .long 0x1badb002
  .long 0x00010000
  .long (-0x1badb002 -0x00010000) # checksum
  .long mboot_load_addr # header_addr
  .long mboot_load_addr
  .long mboot_load_end
  .long mboot_bss_end
  .long mboot_entry_addr

mboot_entry:

# zero 4 pages for our bootstrap page tables
  xor %eax, %eax
  mov $0x1000, %edi
  mov $0x5000, %ecx
  rep stosb

# P4ML[0] -> 0x2000 (PDPT-A)
  mov $(0x2000 | 3), %eax
  mov %eax, 0x1000

# P4ML[511] -> 0x3000 (PDPT-B)
  mov $(0x3000 | 3), %eax
  mov %eax, 0x1FF8

# PDPT-A[0] -> 0x4000 (PD)
  mov $(0x4000 | 3), %eax
  mov %eax, 0x2000

# PDPT-B[510] -> 0x4000 (PD)
  mov $(0x4000 | 3), %eax
  mov %eax, 0x3FF0

# PD[0..511] -> 0..1022MB
  mov $0x83, %eax
  mov $0x4000, %ebx
  mov $512, %ecx
ptbl_loop:
  mov %eax, (%ebx)
  add $0x200000, %eax
  add $0x8, %ebx
  dec %ecx
  jnz ptbl_loop

# Clear ebx for initial processor boot.
# When secondary processors boot, they'll call through
# entry32mp (from entryother), but with a nonzero ebx.
# We'll reuse these bootstrap pagetables and GDT.
  xor %ebx, %ebx

.global entry32mp
entry32mp:
# CR3 -> 0x1000 (P4ML)
  mov $0x1000, %eax
  mov %eax, %cr3

  lgdt (gdtr64 - mboot_header + mboot_load_addr)

# Enable PAE - CR4.PAE=1
  mov %cr4, %eax
  bts $5, %eax
  mov %eax, %cr4

# enable long mode - EFER.LME=1
  mov $0xc0000080, %ecx
  rdmsr
  bts $8, %eax
  wrmsr

# enable paging
  mov %cr0, %eax
  bts $31, %eax
  mov %eax, %cr0

# shift to 64bit segment
  ljmp $8,$(entry64low - mboot_header + mboot_load_addr)

.align 16
gdtr64:
  .word gdt64_end - gdt64_begin - 1;
  .quad gdt64_begin - mboot_header + mboot_load_addr

.align 16
gdt64_begin:
  .long 0x00000000 # 0: null desc
  .long 0x00000000
  .long 0x00000000 # 1: Code, R/X, Nonconforming
  .long 0x00209800
  .long 0x00000000 # 2: Data, R/W, Expand Down
  .long 0x00009000
gdt64_end:

.align 16
.code64
entry64low:
  movq $entry64high, %rax
  jmp *%rax

.global _start
_start:
entry64high:

# ensure data segment registers are sane
  xor %rax, %rax
  mov %ax, %ss
  mov %ax, %ds
  mov %ax, %es
  mov %ax, %fs
  mov %ax, %gs

# check to see if we're booting a secondary core
  test %ebx, %ebx
  jnz entry64mp

# setup initial stack
  mov $0xFFFFFFFF80010000, %rax
  mov %rax, %rsp

# enter main()
  jmp main

.global __deadloop
__deadloop:
# we should never return here...
  jmp .

entry64mp:
# obtain kstack from data block before entryother
  mov $0x7000, %rax
  mov -16(%rax), %rsp

.global wrmsr
wrmsr:
  mov %rdi, %rcx # arg0 -> msrnum
  mov %rsi, %rax # val.low -> eax
  shr $32, %rsi
  mov %rsi, %rdx # val.high -> edx
  wrmsr
  retq
