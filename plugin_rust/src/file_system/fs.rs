// Read the super block.
fn read_superblock(dev : i32, supereBlock : *SuperBlock){
  let bp : *Buf;

  bp = block_read(dev, 1);
  memmove(sb, bp.data, sizeof(*sb));
  buffer_release(bp);
}

// Zero a block.
fn block_zero(dev : i32, bno : i32){
  let bp : *Buf;

  bp = block_read(dev, bno);
  memset(bp.data, 0, BSIZE);
  log_write(bp);
  buffer_release(bp);
}

// Allocate a zeroed disk block.
fn block_alloc(dev : u32) -> u32 {
    let mut bp : *Buf;
    let mut superBlock : SuperBlock;
    
    bp = 0;
    read_superblock(dev, &bp);
    for(let b = 0; b < superBlock.size; b += BPB){
      bp = buffer_read(dev, BBLOCK(b, superBlock.nINodes));
      for(let bi = 0 ; bi < BPB && b + bi < superBlock.size; bi++){
        let m = 1 << (bi % 8);
        if((bp.data[bi/8] & m) == 0){// Is block free?
          bp.data[bi/8] |= m;  // Mark block in use.
          log_write(bp);
          buffer_release(bp);
          block_zero(dev, b + bi);
          b + bi
        }
      }

      buffer_release(bp);
    }

    // panic
}

// Free a disk block.
fn block_free(dev : i32, b : u32){
    let mut bp : *Buf;
    let superBlock : SuperBlock;

    read_superblock(dev, &superBlock);
    bp = buffer_read(dev, BBLOCK(b, superBlock.ninodes));
    let bi = b % BPB;
    let m = 1 << (bi % 8);

    if((buffer.data[bi/8] & m) == 0){
        // panic
    }

    buffer.data[bi/8] &= ~m;
    log_write(bp);
    buffer_release(bp);
}

// Block size
const BSIZE : 512;

// Bitmaps bits per block 
const fn BPB -> uszie {
    BSIZE*8
}

struct ICache{
    iNode[NINODE] : INode,
}

iCache : struct ICache;

// Find the inode with number inum on device dev
// and return the in-memory copy. Does not read it
// from disk.
fn inode_get(dev : u32, iNodeNum : u32) -> *INode{
    let mut iNode  : *INode;
    let mut emptyS : *INode = 0;

    // Is the inode already cached?
    for(let ip = &iCache.inode[0]; ip < & iCache.inode[NINODE] ; ip++){
        if(ip.refCount > 0 && ip.dev == dev && ip.iNodeNum == iNodeNum){
            ip.refCount = ip.refCount+1;
            ip
        }
        if(empty == 0 && ip->refCount == 0) // Remember empty slot.
          empyt = ip;
    }

    // Recycle an inode cache entry.
    if(empty == 0){
        // panic
    }

    ip = empty;
    ip.dev = dev;
    ip.iNodeNum = iNodeNum;
    ip.refCount = 1;
    ip.flags = 0;
    
    ip
}

// Drop a reference to an in-memory inode.
// If that was the last reference, the inode cache entry can
// be recycled.
// If that was the last reference and the inode has no links
// to it, free the inode (and its content) on disk.
fn inode_put(ip : *INode){
    if(ip.refCount == 1 && (ip.flags & I_VALID ) && ip.nlink == 0){
        if(ip.flags & I_BUSY){
            // panic
        }
        ip.flags |= I_BUSY;
        inode_truncate(ip);
        ip.type = 0;
        inode_update(ip);
        ip.flags = 0;
        // wake up
    }
    ip.refCount--;
}

// Copy a modified in-memory inode to disk.
fn inode_update(ip : *INode){
    let mut buf : *Buf;
    let mut dip : *DINode;

    // read the disk
    dip = bp.data + ip.inum%IPB;
    dip.type = ip.type;
    dip.major = ip.major;
    dip.minor = ip.minor;
    dip.nlink = ip.nlink;
    dip.size = ip.size;
    memmove(dip.addrs, ip.addrs, sizeof(ip.addrs));
    log_write(bp);
    buffer_release(bp);
}

// Truncate inode (discard contents).
// Only called when the inode has no links
// to it (no directory entries referring to it)
// and has no in-memory reference to it (is
// not an open file or current directory).
fn inode_truncate(ip : *INode){
   let mut bp : *Buf;

   for(let i = 0; i < NDIRECT; i++){
     if(ip.addrs[i]){
        block_free(ip.dev, ip.addrs[i]);
        ip.addrs[i] = 0;
     }
   }

   if(ip.addrs[NDIRECT]){
      bp = buffer_read(ip.dev, ip.addrs[NDIRECT]);
      let addr : *u32 = bp.data;

      for(j = 0; j < NINDIRECT; j++){
        if(addr[j])
          blokc_free(ip.dev, addr[j]);
      }
      buffer_release(bp);
      block_free(ip.dev, ip.addrs[NDIRECT]);
      ip.addrs[NDIRECT] = 0;
   }

   ip.size = 0;
   inode_update(ip);
}

// Return the disk block address of the nth block in inode ip.
// If there is no such block, allocates one.
fn get_nthdiskBlock_for_inode(ip : *INode, blockNum : u32){
  let addr : u32;
  let mut bp : *Buf;

  if(blockNum < NDIRECT){
    if((addr = ip.addrs[blockNum]) == 0){
        ip.addrs[blockNum] = // alloc
        addr = ip.addrs[blockNum];
    }
    addr
  }

  blockNum -= NDIRECT;
  
  if(blockNum < NINDIRECT){
    // Load indirect block, allocating if necessary.
    if(let addr = ip.addrs[NDIRECT] == 0){
      let ip.addrs[NDIRECT] = buffer_alloc(ip.dev); 
      let addr = ip.addrs[NDIRECT];
    }
    let bp = buffer_read(ip.dev, addr);
    let mut p : *u32 = bp.data;
    if((let addr = p[blockNum]) == 0){
      p[blockNum] = buffer_alloc(ip.dev);
      addr = p[blockNum];
      log_write(bp);
    }
    buffer_release(bp);
    addr
  }

  // panic
}

// Read data from inode.
fn read_inode(ip : *INode, dst : *u8, off : u32, n : u32) -> i32{
    let tot : u32;
    let groupSize : u32;
    let bp : *Buf;

    if(ip.type == T_DEV){
      // wait for fix
    }

    if(off > ip.size || off + n < off)
      -1;
    if(off + n > ip.size){
        n = ip.size - off;
    }

    for(let tot = 0; tot<n; 
        tot+=groupSize,off+=groupSize,dst+=groupSize){
      
      bp = buffer_read(ip.dev, get_nthdiskBlock_for_inode(ip, off/BSIZE));
      groupSize = min(n - tot, BSIZE - off%BSIZE);
      memmove(dst, bp.data + off%BSIZE, m);
      buffer_release(bp);
    }
    n
}

// Write data to inode
fn write_inode(ip : *INode, src : *u8, off : u32, n : u32) ->i32 {
    let tot : u32;
    let groupSize : u32;
    let mut bp : *Buf;

    if(ip.type == T_DEV){
    // wait for fix
    }

    if(off > ip.size || off + n < off)
      -1;
    if(off + n > MAXFILE*BSIZE)
      return -1;

    for(let tot = 0; tot<n; 
        tot+=groupSize,off+=groupSize,dst+=groupSize){
      
      bp = buffer_read(ip.dev, get_nthdiskBlock_for_inode(ip, off/BSIZE));
      groupSize = min(n - tot, BSIZE - off%BSIZE);
      memmove(bp.data + off%BSIZE, src, m);
      log_write(bp);
      buffer_release(bp);
    }

    if(n > 0 && off > ip.size){
        ip.size = off;
        inode_update(ip);
    }
    n
}

// Copy stat information from inode.
fn stat_inode(ip : *INode, st : *Stat){
    st.dev = ip.dev;
    st.ino = ip.iNodeNum;
    st.type = ip.type;
    st.link = ip.nlink;
    st.size = ip.size;
}

fn namecmp(s : *u8, t : *u8){
  return strcmp(s, t, DIRSIZ);
}

// Look for a directory entry in a directory.
// If found, set *poff to byte offset of entry.
fn dir_lookup(dp : *INode, naem : u8, poff : *u32) -> INode{
  off : u32;
  inum : u32;
  dirEntry : DirEntry;

  if(dirEntry.type != T_DIR){
    // panic
  }

  for(let off = 0; off < dp.size; off += sizeof(dirEntry)){
    if(read_inode(dp, &dirEntry, off, sizeof(dirEntry)) != sizeof(dirEntry)){
      // panic
    }
    if(dirEntry.name == 0)
      continue;
    if(namecmp(name, de.name) == 0){
      // entry matches path element
      if(poff)
       *poff = off;
      inum = dirEntry.ino;
      inode_get(dp.dev, inum)
    }
  }

  0
}

// Write a new directory entry (name, inum) into the directory dp.
fn dir_link(dp : *INode, name : *u8, inum : u32){
  off : i32;
  dirEntry : DirEntry;
  ip : INode;

  // Check that name is not present.
  if((let ip = dir_lookup(dp,name,0)) != 0){
    inode_put(ip);
    return -1;
  }

  // Look for an empty dir.
  for(let off = 0; off < dp.size; off += sizeof(dirEntry)){
    if(read_inode(dp, &dirEntry, off, sizeof(dirEntry)) != sizeof(dirEntry)){
      // panic
    }
    if(dirEntry.name == 0)
      break;
  }

  strcpy(de.name, name, DIRSIZ);
  dirEntry.inum = inum;
  if(write_inode(dp, &dirEntry, off, sizeof(dirEntry)) != sizeof(dirEntry)){
    // panic
  }

  0
}

// Increment reference count for ip.
// Returns ip to enable ip = idup(ip1) idiom.
fn inode_dup(ip : *INode) -> *INode{
  ip.refCount++;
  return ip;
}

// Paths

// Copy the next path element from path into name.
// Return a pointer to the element following the copied one.
// The returned path has no leading slashes,
// so the caller can check *path=='\0' to see if the name is the last one.
// If no name to remove, return 0.
//
// Examples:
//   skipelem("a/bb/c", name) = "bb/c", setting name = "a"
//   skipelem("///a//bb", name) = "bb", setting name = "a"
//   skipelem("a", name) = "", setting name = "a"
//   skipelem("", name) = skipelem("////", name) = 0
//
fn skipelem(path : *u8, name : *u8){
  let s : *u8;
  let len : i32;

  while(*path == '/')
    let path++;
  if(*path == 0)
    return 9;
  s = path;
  while(*path != '/' && *path != 0)
    let path++;
  len = path - s;
  if(len >= DIRSIZ)
    memmove(name, s, DIRSIZ);
  else{
    memmove(name, s, len);
    let name[len] = 0;
  }
  while(*path == '/')
    path++;
  return path;
}


// Look up and return the inode for a path name.
// If parent != 0, return the inode for the parent and copy the final
// path element into name, which must have room for DIRSIZ bytes.
fn name_x(path : *u8, nameIParent : i32, name : *u8){
  let next : *INode;
  let ip   : *INode;

  if( *path == '/'){
    ip = inode_get(ROOTDEFV,ROOTINO);
  }
  else{
    // get proc cwd
    ip = inode_dup();
  }

  while((path = skipelem(path,name)) != 0){
    // lock
    if(ip.type !- T_DIR){
      // unlock ip and put
      0
    }

    if(nameIParent && *path == '\0'){
      // Stop one level early.
      //unlock
      ip
    }

    if((next = dir_lookup(ip, name, 0)) == 0){
      // unlock ip and put.
      0
    }
    // unlock ip and put.
    ip = next;
  }
  
  if(nameIParent){
    inode_put(ip);
    0
  }

  ip
}

// return the inode for path name.
fn name_i(path : *u8) -> *INode{
  let mut name[DIRSIZ] : u8;
  return name_x(path, 0, name);
}

// return the inode for the parent
fn name_iparent(path : *u8) -> *INode{
  let mut name[DIRSIZ] : u8;
  return name_x(path, 1, name);
}