#pragma once

#define CR0_MONITOR_COPROC  BIT(1)  /* Trap on FPU "WAIT" commands. */
#define CR0_EMULATION       BIT(2)  /* Enable OS emulation of FPU. */
#define CR0_TASK_SWITCH     BIT(3)  /* Trap on any FPU usage, for lazy FPU. */
#define CR0_NUMERIC_ERROR   BIT(5)  /* Internally handle FPU problems. */
#define CR0_WRITE_PROTECT   BIT(16) /* Write protection in supervisor mode. */
#define CR4_PCE             BIT(8)  /* Performance-Monitoring Counter enable. */
#define CR4_OSFXSR          BIT(9)  /* Enable SSE et. al. features. */
#define CR4_OSXMMEXCPT      BIT(10) /* Enable SSE exceptions. */
#define CR4_OSXSAVE         BIT(18) /* Enavle XSAVE feature set */
#define CR4_VMXE            BIT(13) /* Enable VMX mode. */
#define CR4_SMEP            BIT(20) /* Supervisor Mode Execution Prevention. */
#define CR4_SMAP            BIT(21) /* Supervisor Mode Access Prevention. */

#define FLAGS_TF            BIT(8)  /* Trap Flag */
#define FLAGS_IF            BIT(9)  /* Interrupt enable Flag */
#define FLAGS_HIGH          BIT(1)  /* Bits in the FLAGS register that must be high */
#define FLAGS_LOW           (BIT(3) | BIT(5)) /* Bits in the FLAGS register that must be low */
#define FLAGS_MASK          MASK(12)/* Only the first 12 bits of the FLAGS are used, rest should be zero */
#define FLAGS_USER_DEFAULT  FLAGS_IF | FLAGS_HIGH
