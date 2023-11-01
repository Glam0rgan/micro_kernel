/* for x86-64, the large page size is 2 MiB and huge page size is 1 GiB */
#define os_WordBits           64
#define os_WordSizeBits       3
#define os_PageBits           12
#define os_SlotBits           5
#if CONFIG_XSAVE_SIZE >= 832
#define os_TCBBits         12
#else
#define os_TCBBits         11
#endif