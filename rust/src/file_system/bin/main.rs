fn main(){
  file_system_init();
  let mut index = 1;

  let bufferPoint : *OsIPCBuffer = 0x2000;
  let mut msg : OsMessageInfo = 0;

  while(index <= 10){
    call(0,msg);
    receive(1,&msg);
    yield();
  }
}