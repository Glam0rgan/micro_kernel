#include <os.h>

void main(void) {
    int num = 1;
    OsMessageInfo msg;
    receive(0, &msg);
    OsWord id;
    id = os_get_mr(0);
    //printf("receive the id %l", id);
}
