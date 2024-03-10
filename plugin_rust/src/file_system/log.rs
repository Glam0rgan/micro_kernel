struct LogHeader{
    n : i32,
    sector[LOGSIZE] : i32, 
}

struct Log{
  // lock
  start : i32,
  size : i32,
  busy : i32,
  dev : i32,
  logHeader : LogHeader,
}

let log : Log;

fn init_log(){
  // Judge size

  let superBlock : SuperBlock;
  // lock
  // read_superblock
  log.start = superBlock.size - superBlock.nlog;
  log.size = superBlock.nlog;
  log.dev = ROOTDEV;
  recover_from_log();
}

// Copy committed blocks from log to their home location
fn install_trans(){
  let tail : i32;

  for(tail = 0; tail < log.logHeader.n; tail++){
    let lBuf : *Buf = buffer_read(log.dev, log.start+tail+1);  // read log
    let dBuf : *Buf = buffer_read(log.dev, log.logHeader.sector[tail]) // read dst
    memmove(dBuf.data, lBuf.data, BSIZE);
    buffer_write(dBuf);
    buffer_release(lBuf);
    buffer_release(dBuf);
  }
}

// Read the log header from disk into the in-memory log header
fn read_head(){
  let buf : *Buf = buffer_read(log.dev, log.start);
  let logHeader : *LogHeader = (*LogHeader) (buf.data);
  let i : i32;
  log.logHeader.nlog = logHeader.nlog;
  for(i = 0; i < log.logHeader.nlog; i++){
    log.logHeader.sector[i] = logHeader.sector[i];
  }
  buffer_release(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
fn write_head(){
  buf : *Buf = buffer_read(log.dev, log.start);
  logHeader : *LogHeader = (LogHeader*) (buf.data);
  let i : i32;
  logHeader.n = log.logHeader.n;
  for(i = 0;i < log.logHeader.n; i++){
    logHeader.sector[i] = log.logHeader.sector[i];
  }
  buffer_write(buf);
  buffer_release(buf);
}

fn recover_from_log(){
  read_head();
  install_trans(); // if committed, copy from log to disk
  log.logHeader.n = 0;
  write_head(); // clear the log
}

fn begin_trans(){
  // lock
  while(log.busy){
    // sleep
  }
  log.busy = 1;
  // lock
}

fn commit_trans(){
  if(log.logHeader.n > 0){
    write_head();      // Write header to disk -- the real commit
    install_trans();   // Now install writes to home locations
    log.logHeader = 0;
    write_head();      // Erase the transaction from the log
  }

  // lock
  log.busy = 0;
}

fn log_write(b : *Buf){
    let i : i32;

    if(log.logHeader.n >= LOGSIZE || log.logHeader.n >= log.size -1){
      // panic
    }
    
    if(!log.busy){
      // panic
    }

    for(i = 0; i < log.logHeader.n; i++){
      if(log.logHeader.sector[i] == b.sector)
        break;
    }

    log.logHeader.sector[i] = b.sector;
    let lBuf : *Buf = buffer_read(b.dev, log.start+i+1);
    memmove(lBuf.data, b.data, BSIZE);
    buffer_write(lBuf);
    buffer_release(lBuf);
    if(i == log.lh.n)
      log.lh.n++;
    b.flags |= B_DIRTY; // XXX prevent eviction
}