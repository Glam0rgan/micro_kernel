#include <os.h>

void main(void) {
    int num = 1;
    OsMessageInfo msg;
    receive(0, &msg);
    OsWord id;
    OsIPCBuffer* bufferPoint = 0x2000;
    id = bufferPoint->msg[0];
    test();
    print(id);
    exit();
    //printf("receive the id %l", id);
}
