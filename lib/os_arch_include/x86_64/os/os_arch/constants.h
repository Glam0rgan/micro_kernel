#if CONFIG_XSAVE_SIZE >= 832
#define os_TCBBits         12
#else
#define os_TCBBits         11
#endif