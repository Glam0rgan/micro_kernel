#include <os.h>

void main(void) {
    OsMessageInfo msg;
    msg.capsUnwrapped = 0;
    msg.label = 1;
    msg.extraCaps = 0; 
    msg.length = 1;
    OsWord id = 155;
    OsIPCBuffer* bufferPoint = 0x2000;
    //os_set_ipcBuffer(bufferPoint);
    //os_set_mr(0, id);
    bufferPoint->msg[0]=id;
    send(0, msg);
    test();
    exit();
}
