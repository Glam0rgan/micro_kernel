
fn main(){
  allocator_init();
  let bufferPoint : *OsIPCBuffer = 0x2000;
  let mut msg : OsMessageInfo = 0;

  while(true){

    // Receive the memory request and mmap the memory.
    receive(0,&msg);
    let mut id = bufferPoint.msg[0];
    let mut funId = bufferPoint.msg[1];
    if(funId == 1){
      // judge the corret return value
      mmap(id, physical_memory_alloc());
      // Set the msg.
      // Set the IPCBuffer.
      reply(msg);
    }
    else{
      // Set the msg.
      // Set the IPCBuffer.
      reply(msg);
    }
  }    
}