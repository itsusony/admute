#define PORT 5000
#define PORT_FOR_PARSER 5001

#define ERRMSG_SOCKET "Could not create socket"
#define ERRMSG_BIND   "Could not bind to address"
#define ERRMSG_LISTEN "Could not listen on address"
#define ERRMSG_ACCEPT "Socket accept error"
#define SPLIT_POS 512

#define SO_CHECK 0
#define SO_WRITE 1

#include "lib/uthash.h"

int socket_main;
int socket_in1;

int socket_parser;
int socket_in2;

typedef struct socket_obj {
    int type; // check==0, write==1
    char userid[128];
    char adid[128];
} SOCKET_OBJ;

typedef struct mem_obj {
    char *userid; // key
    char **adids;
    long adids_item_len;
    long adids_mem_len;
    UT_hash_handle hh;
} MEM_OBJ;

MEM_OBJ* cache = NULL;

pthread_t p1;
