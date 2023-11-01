
/* These are the indices of the registers in the
 * saved thread context. The values are determined
 * by the order in which they're saved in the trap
 * handler.
 *
 * BEWARE:
 * You will have to adapt traps.S extensively if
 * you change anything in this enum!
 */

 /* This register layout is optimized for usage with
  * the syscall and sysret instructions. Interrupts
  * and sysenter have to do some juggling to make
  * things work */

enum _register {
  // User registers that will be preserved during syscall
  // Deliberately place the cap and badge registers early
  // So that when popping on the fastpath we can just not
  // pop these
  RDI = 0,    /* 0x00 */
  capRegister = 0,
  badgeRegister = 0,
  RSI = 1,    /* 0x08 */
  msgInfoRegister = 1,
  RAX = 2,    /* 0x10 */
  RBX = 3,    /* 0x18 */
  RBP = 4,    /* 0x20 */
  R12 = 5,    /* 0x28 */
  R13 = 6,    /* 0x30 */
  R14 = 7,    /* 0x38 */
  RDX = 8,    /* 0x40 */
  // Group the message registers so they can be efficiently copied
  R10 = 9,    /* 0x48 */
  R8 = 10,   /* 0x50 */
  R9 = 11,   /* 0x58 */
  R15 = 12,   /* 0x60 */
  FLAGS = 13,   /* 0x68 */
  // Put the NextIP, which is a virtual register, here as we
  // need to set this in the syscall path
  NextIP = 14,   /* 0x70 */
  // Same for the error code
  Error = 15,   /* 0x78 */
  // Kernel stack points here on kernel entry 
  RSP = 16,   /* 0x80 */
  FaultIP = 17,   /* 0x88 */
  // Now user Registers that get clobbered by syscall
  R11 = 18,   /* 0x90 */
  RCX = 19,   /* 0x98 */
  CS = 20,   /* 0xa0 */
  SS = 21,   /* 0xa8 */
  n_immContextRegisters = 22,   /* 0xb0 */

  // For locality put these here as well
  FS_BASE = 22,   /* 0xb0 */
  TLS_BASE = FS_BASE,
  GS_BASE = 23,   /* 0xb8 */

  n_contextRegisters = 24    /* 0xc0 */
};