#pragma once

#include <os/macros.h>

#include <os/os_arch/simple_types.h>

#if defined(OsINT64_IS_LONG)
#define _Osint64_type    long int
#define _Osint64_fmt     l  // printf() formatting, integer suffix
#elif defined(OsINT64_IS_LONG_LONG)
#define _Osint64_type    long long int
#define _Osint64_fmt     ll  // printf() formatting, integer suffix
#else
#error missing definition for 64-bit types
#endif

typedef signed _Osint64_type     OSInt64;
typedef unsigned _Osint64_type   OSUint64;

// Define OsWord.
//#if defined(OsWORD_IS_UINT32)
//typedef OSUint32 OsWord;
//#define _Osword_fmt // empty
//#elif defined(OsWORD_IS_UINT64)

//#else
//#error missing definition for OsWord type
//#endif

typedef OSUint64 OsWord;
#define _Osword_fmt _Osint64_fmt
typedef OsWord OsCPtr;