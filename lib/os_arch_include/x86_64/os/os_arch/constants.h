#pragma once

/* for x86-64, the large page size is 2 MiB and huge page size is 1 GiB */
#define OsWordBits           64
#define OsWordSizeBits       3
#define OsPageBits           12
#define OsSlotBits           5
#define OsLargePageBits      21
#define OsHugePageBits       30

#if CONFIG_XSAVE_SIZE >= 832
#define  OsTCBBits         12
#else
#define OsTCBBits         11
#endif

#define OsEndpointBits 4
#define OsNotificationBits 5
