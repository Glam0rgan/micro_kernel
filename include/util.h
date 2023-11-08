

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

#define PASTE(a, b) a ## b

#ifdef __ASSEMBLER__

#define UL_CONST(x) x
#define ULL_CONST(x) x
#define NULL 0

#else

#define UL_CONST(x) PASTE(x, ul)
#define ULL_CONST(x) PASTE(x, llu)
#define NULL ((void *)0)

#endif

#define BIT(n) (UL_CONST(1) << (n))
#define MASK(n) (BIT(n) - UL_CONST(1))

#ifndef __ASSEMBLER__

#define CONST    __attribute__((__pure__))
#define PURE     __attribute__((__pure__))
#define NORETURN __attribute__((__noreturn__))

#endif