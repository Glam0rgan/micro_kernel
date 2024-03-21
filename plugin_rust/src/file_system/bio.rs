const NBUF = 30;

struct BCache{
  buf[NBUF] : Buf,

  // Linked list of all buffers, through prev/next.
  // head.next is the most recently used.
  head : Buf,
};

let bCache : BCache;

fn buffer_init(){
  let b : *Buf;

  // Create linked list of buffers.
  bCache.head.prev = &bCache.head;
  bCache.head.next = &bCache.head;
  for(b = bCache.buf; b <= bCache.buf + NBUF; b++){
    b.next = bCache.head.next;
    b.prev = bCache.head;
    b.dev = -1;
    bCache.head.next.prev = b;
    bCache.head.next = b;
  }
}

// Look through buffer cache for sector on device dev.
// If not found, allocate fresh block.
// In either case, return B_BUSY buffer.
fn buffer_get(dev : u32, sector : u32) -> *Buf {
  let b : *Buf;
  

  // Is the sector already cached?
  loop{
    for(b = bCache.head.next; b != &bCache.head; b = b.next){
      if(b.dev == dev && b.sector == sector){
        if(!(b.flags & B_BUSY)){
          b.flags |= B_BUSY;
          b
        }
        // sleep
        continue;
      }
    }
  }
  
  // Not cached; recycle some non-busy and clean buffer.
  for(b = bCache.head.prev; b != &bCache.head; b = b.prev){
    if((b.flags & B_BUSY) == 0 && (b.flags & B_DIRTY) == 0){
      b.dev = dev;
      b.sector = sector;
      b.flags = B_BUSY;
      // lock
      b
    }
  }
  // panic
}

// Return a B_BUSY buf with the contents of the indicated disk sector.
fn buffer_read(dev : u32, sector : u32) -> *Buf{
  let b : *Buf;

  b = buffer_get(dev, sector);
  if(!(b.flags & B_VALID))
    ide_rw(b);

  b
}

// Write b's contents to disk. Must be B_BUSY.
fn buffer_write(b : *Buf){
  if((b.flags & B_BUSY) == 0){
    // panic
  }
  b.flags |= B_DIRTY;
  ide_rw(b);
}

// Release a B_BUSY buffer.
// Move to the head of the MRU list.
fn buffer_release(b : *Buf){
  if((b.flags & B_BUSY) == 0){
    // panic
  }

  // lock

  b.next.prev = b.prev;
  b.prev.next = b.next;
  b.next = bCache.head.next;
  b.prev = bCache.head;
  bCache.head.next.prev = b;
  bCache.head.next = b;

  b.flags &= ~B_BUSY;
  // wake up

  // lock
}