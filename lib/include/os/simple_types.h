#pragma once

#include <os/macros.h>

#include <os/arch/simple_types.h>

// Define OsWord.
#if defined(OS_WORD_IS_UINT32)
typedef OSUint32 OsWord;
#define _os_word_fmt // empty
#elif defined(OS_WORD_ISUINT64)
typedef OSUint64 OsWord;
#define _os_word_fmt _os_int64_fmt
#else
#error missing definition for OsWord type
#endif
typedef OsWord OsCPtr;