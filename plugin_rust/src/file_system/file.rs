
let fTable : FTabel;

// Allocate a file structure.
fn file_alloc() -> *File {
  let f : *File;

  for(f = fTable.file; f < fTable.file + NFILE; f++){
    if(f.refCount == 0){
        f.refCount = 1;
        f
    }
  }
  0
}

// Increment ref count for file f.
fn file_dup(f : *File) -> *File{
    if(f.refCount < 1){
        // panic
    }
    f.refCount++;
    f
}

// Choose file f.
// Decrement ref count, close when reaches 0.
file_close(f : *File){
    let mut ff : File;

    if(f.refCount < 1){
        // panic
    }
    if(--f.refCount > 0){
        return;
    }

    ff = *f;
    f.ref = 0;
    f.type = FD_NONE;

    if(ff.type == FD_PIPE){

    }
    else if(ff.type == FD_INODE){
      begin_trans();
      inode_put(ff.ip);
      commit_trans();
    }
}

// Get metadata about file f.
fn file_stat(f : *File, st : *Stat){
    if(f.type == FD_INODE){
        stat_inode(f.ip, st);
        0
    }
    -1
}

// Read from file f.
fn file_read(f : *File, addr : *u8, n : i32){
    let r : i32;

    if(f.readable == 0)
      -1
    if(f.type == FD_PIPE)
      pipe_read(f.pipe, addr, n);
    if(f.type == FD_INODE){
      // lock
      if((r = read_inode(f.ip, addr, f.off, n)) > 0)
        f.off += r;
      // unlock
      r
    }
    // panic
}

// Write to file f.
file_write(f : *File, addr : *u8, n : i32){
    let r : i32;

    if(f.writable == 0)
      -1
    if(f.type == FD_PIPE)
      pipe_write();
    if(f.type == FD_INODE){
      // write a few blocks at a time to avoid exceeding
      // the maximum log transaction size, including
      // i-node, indirect block, allocation blocks,
      // and 2 blocks of slop for non-aligned writes.
      // this really belongs lower down, since writei()
      // might be writing a device like the console.
      let max : i32 = ((LOGSIZE -1 -1 -2) / 2) * 512;
      let i = 0;
      while(i < n){
        let n1 = n - i;
        if(n1 > max)
          n1 = max;
        
        begin_trans();
        // lock
        if((r = write_inode(f.ip, addr + i, f.off, n1) > 0)
          f.off == r;
        // unlock
        commit_trans();

        if(r <  0)
         break;
        if(r != n1){
            // panic
        }
        i += r;
      }
      if(i == n)
        n
      else
        -1
    }
    // panic
}