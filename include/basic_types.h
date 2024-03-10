#pragma once

#include <arch/types.h>
#include <stdint.h> // define fixed-width integer
#include <stdbool.h> // define boolean keywords: true, false, bool

// unsigned integer
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// signed integer
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

/* for lib headers that the kernel shares */
typedef uint8_t Osu8;
typedef uint16_t Osu16;
typedef uint32_t Osu32;

typedef NodeId OsNodeId;
typedef PAddr OsPAddr;
typedef Dom OsDomain;
