#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "define.c"
#include "lib/uthash.h"

void connect_srv() {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    socket_main = socket(AF_INET, SOCK_STREAM, 0);
    connect(socket_main, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
}

int main(int argc, char** argv){

    char buf[16]="";

    // write
    {
        connect_srv();

        SOCKET_OBJ *obj = calloc(1, sizeof(SOCKET_OBJ));
        obj->type=1;
        strcat(obj->userid, "user1");
        strcat(obj->adid, "adid1");

        if (send(socket_main, obj, sizeof(SOCKET_OBJ), 0) != -1) {
            memset(buf, 0, sizeof(buf));
            if (read(socket_main, buf, sizeof(buf)) != -1) {
                fprintf(stderr, "write: %s, need ok\n", buf);
            }
        }

        free(obj);
        close(socket_main);
    }

    // check adid by userid, adid, result is exist(1)
    {
        connect_srv();

        SOCKET_OBJ *obj = calloc(1, sizeof(SOCKET_OBJ));
        obj->type=0;
        strcat(obj->userid, "user1");
        strcat(obj->adid, "adid1");

        if (send(socket_main, obj, sizeof(SOCKET_OBJ), 0) != -1) {
            memset(buf, 0, sizeof(buf));
            if (read(socket_main, buf, sizeof(buf)) != -1) {
                fprintf(stderr, "check1: %s, need 1\n", buf);
            }
        }

        close(socket_main);
    }

    // check adid by userid, adid, result is empty(0)
    {
        connect_srv();

        SOCKET_OBJ *obj = calloc(1, sizeof(SOCKET_OBJ));
        obj->type=0;
        strcat(obj->userid, "user999");
        strcat(obj->adid, "adid999");

        if (send(socket_main, obj, sizeof(SOCKET_OBJ), 0) != -1) {
            memset(buf, 0, sizeof(buf));
            if (read(socket_main, buf, sizeof(buf)) != -1) {
                fprintf(stderr, "check2: %s, need 0\n", buf);
            }
        }

        close(socket_main);
    }

    // error when userid is empty
    {
        connect_srv();

        SOCKET_OBJ *obj = calloc(1, sizeof(SOCKET_OBJ));
        obj->type=0;
        obj->userid[0] = 0;
        strcat(obj->adid, "adid999");

        if (send(socket_main, obj, sizeof(SOCKET_OBJ), 0) != -1) {
            memset(buf, 0, sizeof(buf));
            if (read(socket_main, buf, sizeof(buf)) != -1) {
                fprintf(stderr, "error1: %s, need err\n", buf);
            }
        }

        close(socket_main);
    }

    // error when adid is empty
    {
        connect_srv();

        SOCKET_OBJ *obj = calloc(1, sizeof(SOCKET_OBJ));
        obj->type=0;
        strcat(obj->userid, "user1");
        obj->adid[0] = 0;

        if (send(socket_main, obj, sizeof(SOCKET_OBJ), 0) != -1) {
            memset(buf, 0, sizeof(buf));
            if (read(socket_main, buf, sizeof(buf)) != -1) {
                fprintf(stderr, "error2: %s, need err\n", buf);
            }
        }

        close(socket_main);
    }

    // append adid in exist user
    {
        connect_srv();

        SOCKET_OBJ *obj = calloc(1, sizeof(SOCKET_OBJ));
        obj->type=1;
        strcat(obj->userid, "user1");
        strcat(obj->adid, "adid2");

        if (send(socket_main, obj, sizeof(SOCKET_OBJ), 0) != -1) {
            memset(buf, 0, sizeof(buf));
            if (read(socket_main, buf, sizeof(buf)) != -1) {
                fprintf(stderr, "append: %s, need ok\n", buf);
            }
        }

        close(socket_main);
    }

    return 0;
}
