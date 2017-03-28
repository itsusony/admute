int append_adid(MEM_OBJ* mo, const char* new_adid, int dry_mode) {
    long i;
    for(i = 0;i < mo->adids_item_len; i++) {
        char* adid = mo->adids[i];
        if (strcmp(adid, new_adid) == 0) return 1;
    }
    if (!dry_mode) {
        if (mo->adids_item_len == mo->adids_mem_len) {
            mo->adids_mem_len *= 2;
            mo->adids = realloc(mo->adids, mo->adids_mem_len * sizeof(char*));
        }
        mo->adids[mo->adids_item_len++] = strdup(new_adid);
    }
    return 0;
}

int process_obj(const SOCKET_OBJ* obj, char** output_msg) {
    if (!obj || !strlen(obj->userid) || !strlen(obj->adid)){
        strcat(*output_msg, "err");
        return -1;
    }
    MEM_OBJ* found_mo = NULL;
    HASH_FIND_STR(cache, obj->userid, found_mo);

    switch(obj->type) {
        case SO_CHECK:
            if (!found_mo) {
                strcat(*output_msg, "0");
            } else {
                int rst = append_adid(found_mo, obj->adid, 1);
                strcat(*output_msg, rst ? "1" : "0");
            }
            break;
        case SO_WRITE:
            if (found_mo) {
                append_adid(found_mo, obj->adid, 0);
            } else {
                MEM_OBJ* new_mo        = calloc(1, sizeof(MEM_OBJ));
                new_mo->userid         = strdup(obj->userid);
                new_mo->adids_item_len = 1;
                new_mo->adids_mem_len  = 32; // init mem size is 32
                new_mo->adids = calloc(new_mo->adids_mem_len, sizeof(char*));
                new_mo->adids[0] = strdup(obj->adid);
                HASH_ADD_KEYPTR(hh, cache, obj->userid, strlen(obj->userid), new_mo);
            }
            strcat(*output_msg, "ok");
            break;
    }
    return 0;
}

void socket_main_init() {
    int yes = 1;

    if((socket_main = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, ERRMSG_SOCKET"\n");
        exit(1);
    }

    struct sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    setsockopt(socket_main, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));

    if(bind(socket_main, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        fprintf(stderr, ERRMSG_BIND"\n");
        exit(1);
    }

    if(listen(socket_main, 9) < 0) {
        fprintf(stderr, ERRMSG_LISTEN"\n");
        exit(1);
    }
}
void socket_main_close() {
    close(socket_main);
}
void socket_main_loop() {
    char *buf;
    SOCKET_OBJ *obj;
    while(1) {
        buf = calloc(16, sizeof(char));
        obj=calloc(1, sizeof(SOCKET_OBJ));
        if((socket_in1 = accept(socket_main, NULL, NULL)) != -1 && recv(socket_in1, obj, sizeof(SOCKET_OBJ), 0) != -1) {
            process_obj(obj, &buf);
            write(socket_in1, buf, strlen(buf));
        } else {
            fprintf(stderr, ERRMSG_ACCEPT"\n");
        }
        free(obj);
        free(buf);
        close(socket_in1);
    }
}

void socket_parser_init() {
    int yes = 1;

    if((socket_parser = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, ERRMSG_SOCKET"\n");
        exit(1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_FOR_PARSER);
    addr.sin_addr.s_addr = INADDR_ANY;

    setsockopt(socket_parser, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));

    if(bind(socket_parser, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        fprintf(stderr, ERRMSG_BIND"\n");
        exit(1);
    }

    if(listen(socket_parser, 9) < 0) {
        fprintf(stderr, ERRMSG_LISTEN"\n");
        exit(1);
    }
}
void socket_parser_loop() {
    char accept_buf[1024];
    char *buf;
    char cmd[1024]="";
    char arg1[1024]="";
    char arg2[1024]="";
    SOCKET_OBJ* obj;
    while(1) {
        obj = NULL;
        buf = calloc(16, sizeof(char));
        memset(accept_buf, 0, sizeof(accept_buf));
        memset(cmd, 0, sizeof(cmd));
        memset(arg1, 0, sizeof(arg1));
        memset(arg2, 0, sizeof(arg2));
        if((socket_in2 = accept(socket_parser, NULL, NULL)) != -1 && recv(socket_in2, accept_buf, sizeof(accept_buf), 0) != -1) {
            sscanf(accept_buf, "%s %s %s", cmd, arg1, arg2);

            if (strlen(cmd)) {
                if (strcmp(cmd, "set") == 0) {
                    obj = calloc(1, sizeof(SOCKET_OBJ));
                    obj->type=SO_WRITE;
                    if (strlen(arg1)) strcat(obj->userid, arg1);
                    if (strlen(arg2)) strcat(obj->adid, arg2);
                    process_obj(obj, &buf);
                    write(socket_in2, buf, strlen(buf));
                }
                else if (strcmp(cmd, "check") == 0) {
                    obj = calloc(1, sizeof(SOCKET_OBJ));
                    obj->type=SO_CHECK;
                    if (strlen(arg1)) strcat(obj->userid, arg1);
                    if (strlen(arg2)) strcat(obj->adid, arg2);
                    process_obj(obj, &buf);
                    write(socket_in2, buf, strlen(buf));
                }
            }

            if (obj == NULL) {
                write(socket_in2, "err", 3);
            } else {
                free(obj);
            }
        } else {
            fprintf(stderr, ERRMSG_ACCEPT"\n");
        }
        close(socket_in2);
        free(buf);
    }
}
void socket_parser_close() {
    close(socket_parser);
}

void* pthread_p1_work(void* p) {
    socket_parser_init();
    socket_parser_loop();
    socket_parser_close();
    return NULL;
}

void pthread_init() {
    pthread_create(&p1, NULL, (void*)pthread_p1_work, (void*)NULL);
}

void pthread_close() {
    pthread_join(p1, NULL);
}
