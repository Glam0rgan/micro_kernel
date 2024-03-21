const NDIRECT : 12 //
const BSIZE   : 512 // block size
const NINDIRECT : (BSIZE / 4) 
const ROOTINO : 1  // root i-number
const NINODE  : 50 // maxinum number of active i-nodes
const IPB     : BSIZE / DINode // ???

const LOGSIZE : 10

const I_BUSY : 0x1
const I_VALID : 0x2

const NAME_LEN : 14

const T_DIR = 1;
const T_FILE = 2;
const T_DEV = 3;

// Block containing bit for block b
const fn BBLOCK(b : i32, ninodes : u16) -> u32 {
    b/BPB + (ninodes)/IPB + 3
}

// on disk inode
struct DINode{
    type : i16,
    major : i16,
    minor : i16,
    nlink : i16,
    size  : u32,
    addrs[NDIRECT + 1] : u32,
}

// in-memory copy of an inode
struct INode{
    dev : u32, // Device number
    iNodeNum : u32,
    refCount : i32,
    flags : i32,

    type : i16,
    major : i16,
    minor : i16,
    nlink : i16,
    size  : u32,
    addrs[NDIRECT + 1] : u32,
}

struct DirEntry{
    ino : u16,
    name[NAME_LEN] : u8,
}

struct SuperBlock{
    nINodes : u16, // number of inodes
    nZones : u16, // number of zones
    iMapBlk : u16, // the nums of  iMap occupy block
    zMapBlk : u16, // the nums of zMap occupy  block
    fstDataZone : u16, // the first data zone block num
    logZoneSize : u16, // a virtul block size = 1024 << logZoneSize

    maxSize : u32, // the max file size(byte)
    magic : u16, // magic number
    state : u16,
}

struct Stat{
    type : i16,
    dev : i32,
    ino : u32,
    nlink : i16,
    size : u32,
}

struct Buf{
    flags : i32,
    dev   : u32,
    blockNo : u32,
    refCnt : u32,
    prev : *Buf,  // LRU cache list
    next : *Buf,
    qnext : *Buf, // disk queue
    data[BSIZE] : u8,
}

const B_VALID = 0x2;  // Buffer has been read from disk.
const B_DIRTY = 0x4;  // Buffer needs to be written to disk.

struct File{
  enum type{ FD_NONE, FD_PIPE, FD_INODE},
  refCount : i32,
  readable : u8,
  writable : u8,
  pipe     : *Pipe,
  ip       : *INode,
  off      : u32,
}

struct FTable{
    file[NFILE] : File
}