#include <os.h>

void main(void) {
    int num = 1;
    OsMessageInfo msg;
    receive(0, &msg);
    printf("receive the number %l", msg.label);
}