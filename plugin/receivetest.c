#include <os.h>

void main(void) {
    int num = 1;
    OsCPtr endpoint = get();
    OsMessageInfo msg;
    os_recv(endpoint, &msg);
    num = get_number(msg);
    printf("receive the number %l", num);
}