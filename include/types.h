typedef unsigned long u64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
// capability pointer
typedef u64 cptr_t;

typedef u64 bool_t;

struct extra_caps{
  cte_ptr_t excaprefs[os_MsgMaxExtraCaps];
};
typedef struct extra_caps extra_caps_t;