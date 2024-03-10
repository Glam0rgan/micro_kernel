const IDE_BSY  = 0x80;
const IDE_DRDY = 0x40;
const IDE_DF   = 0x20;
const IDE_ERR  = 0x01;

const IDE_CMD_READ  = 0x20;
const IDE_CMD_WRITE = 0x30;

ideQueue : *Buf;
mut haveDisk1 : i32;

// Wait for IDE disk to become ready.
fn ide_wait(checkErr : i32) -> i32{
  let mut r : i32;

  while(((r = inb(0x1f7)) & (IDE_BSY | IDE_DRDY)) != IDE_DRDY)
    ;
  if(checkErr && (r & (IDE_DF | IDE_ERR)) != 0)
    -1
  0
}

fn ide_init(){
  let mut i : i32;

  // picenable
  // ioapicenable

  ide_wait(0);

  // Check if disk1 is present
  outb(0x1f6, 0xe0 | (1<<4));
  for(i=0;i<1000;i++){
    if(inb(0x1f7) != 0){
      haveDisk1 = 1;
      break;
    }
  }

  // Switch back to disk 0.
  outb(0x1f6, 0xe0 | (0 << 4));
}

// Start the request for b.
fn ide_start(b : *Buf){
  if(b == 0){
    // panic
  }

  ide_wait(0);
  outb(0x3f6, 0);  // generate interrupt
  outb(0x1f2, 1);  // number of sectors
  outb(0x1f3, b.blockNo & 0xff);
  outb(0x1f4, (b.blockNo >> 8) & 0xff);
  outb(0x1f5, (b.blockNo >> 16) & 0xff);
  outb(0x1f6, 0xe0 | ((b.dev & 1) << 4) | ((b.blockNo >> 24) & 0x0f));
  
  if(b.flags & B_DIRTY){
    outb(0x1f7, IDE_CMD_WRITE);
    outsl(0x1f0, b.data, 512/4);
  }
  else{             
    outb(0x1f7, IDE_CMD_READ);
  }
}

// Interrupt handler. 
fn ide_intr(){
  let mut b : *Buf;

  if( (b = ideQueue) == 0){
    return;
  }

  ideQueue = b.qnext;

  // Read data if needed.
  if( !(b.flags & B_DIRTY) && ide_wait(1) >= 0 )
    insl(0x1f0, b.data, 512/4);

  // Wake process waiting for this buf.
  // kernel do this job
  
  // Start disk on next buf in queue.
  if(ideQueue != 0)
    ide_start(ideQueue);

  
}

// Sync b with disk.
// If B_DIRTY is set, write buf to disk, clear B_DIRTY, set B_VALID.
// Else if B_VALID is not set, read buf from disk, set B_VALID.
fn ide_rw(b : *Buf){
  let mut pp : *Buf;

  // lock

  if((b.flags & (B_VALID | B_DIRTY)) == B_VALID){
    // panic
  }

  if(b.dev != 0 && !haveDisk1){
    // panic
  }

  // lock
  // Append b to idequeue
  pp = ideQueue;
  if(ideQueue){
    while(pp.qnext)
      pp = pp.next;
    pp.next = b;
  }
  else{
    ideQueue = b;
  }

  // Start disk if necessary
  if(ideQueue == b)
    ide_start(b);

  // Wait for request to finish.
  while((b.flags & (B_VALID | B_DIRTY)) != B_VALID){
    // sleep
  }
}