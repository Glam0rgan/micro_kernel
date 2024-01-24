#include <os.h>

void main(void) {
    int num = 1;
    OsMessageInfo msg;
    msg.capsUnwrapped = 0;
    msg.label = 1;
    msg.extraCaps = 0;
    msg.length = 1;
    OsWord id = 155;
    os_set_mr(0, id);
    //printf("send the word %l", id);
    send(0, msg);
}
