/*

typedef struct OsMessageInfo_ {
  uint64_t label : 52;
  uint64_t capsUnwrapped : 3;
  uint64_t extraCaps : 2;
  uint64_t length : 7;
}OsMessageInfo;

*/

#[inline]
fn get_osmessageinfo_label(osMessageInfo : u64) -> u64 {
  osMessageInfo & 0x000FFFFFFFFFFFFF;
}

#[inline]
fn get_osmessageinfo_capsUnwrapped(osMessageInfo : u64) -> u64 {
  (osMessageInfo >> 52 ) & 0x7 ;
}

#[inline]
fn get_osmessageinfo_extraCaps(osMessageInfo : u64) -> u64 {
  (osMessageInfo >> 55 ) & 0x2 ;
}

#[inline]
fn get_osmessageinfo_length(osMessageInfo : u64) -> u64 {
  (osMessageInfo >> 57 ) & 0x7F ;
}

#[inline]
fn set_osmessageinfo_label(&mut osMessageInfo : u64, label : u64){
  let mut osMessageInfo = osMessageInfo & 0xFFF0000000000000 | label;
}

#[inline]
fn set_osmessageinfo_capsUnwrapped(&mut osMessageInfo : u64, capsUnwrapped : u64){
  let mut osMessageInfo = osMessageInfo & 0xFF8FFFFFFFFFFFFF | capsUnwrapped;
}

#[inline]
fn set_osmessageinfo_extraCaps(&mut osMessageInfo : u64, extraCaps : u64){
  let mut osMessageInfo = osMessageInfo & 0xFE7FFFFFFFFFFFFF | extraCaps;
}

#[inline]
fn set_osmessageinfo_capsUnwrapped(&mut osMessageInfo : u64, length : u64){
  let mut osMessageInfo = osMessageInfo & 0x01FFFFFFFFFFFFFF | length;
}

/*
enum Os_MsgLimits {
    Os_MsgLengthBits = 7,
    Os_MsgExtraCapBits = 2
};

enum {
    Os_MsgMaxLength = 120,
};

#define LIB_BIT(n)  (1ul<<(n))
#define Os_MsgMaxExtraCaps (LIB_BIT(Os_MsgExtraCapBits)-1)
*/

type OsMessageInfo = u64;
type OsWord = u64;
type OsCPtr = u64;

#[repr(C)]
struct OsIPCBuffer{
    tag : OsMessageInfo,
    msg[Os_MsgMaxLength] : OsWord,
    userData : OsWord,
    caps_or_badges[Os_MsgMaxExtraCaps] : OsWord,
    receiveCNode : OsCPtr,
    receiveIndex : OsCPtr,
    receiveDepth : OsWord,
}

fn main() {
    println!("Hello, world!");
}
