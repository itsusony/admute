#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include "define.c"
#include "func.c"

int main(int argc, char** argv) {
    pthread_init();
    socket_main_init();
    socket_main_loop();
    socket_main_close();
    pthread_close();
    return 0;
}
