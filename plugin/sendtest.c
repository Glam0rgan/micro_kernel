#include <os.h>

void main(void) {
    int num = 1;
    printf("send the number %l", num);
    OsCPtr endpoint = get();
    OsMessageInfo msg = create();
    os_send(endpoint, msg);
}