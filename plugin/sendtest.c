#include <os.h>

void main(void) {
    int num = 1;
    OsMessageInfo msg;
    msg.capsUnwrapped = 0;
    msg.label = 1;
    msg.extraCaps = 0;
    msg.length = 1;
    printf("send the label %l", msg.label);
    send(0, msg);
}