#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include "sft.h"
#include "pdu.h"

int main(int argc, char *argv[])
{
    printf("i am Server !\n");

    SFT_DATA *server = sft_create(SFT_SERVER);
    if(server == NULL) {
        fprintf(stderr, "server : sft_create failed\n");
        exit(EXIT_FAILURE);
    }

    if(0 != server->action->init(server)) {
        fprintf(stderr, "server : sft init failed\n");
        exit(EXIT_FAILURE);
    }

    if(0 < server->action->connect(server, NULL, 9420)) {
        fprintf(stderr, "server : sft connect failed\n");
        exit(EXIT_FAILURE);
    }

    char recv_cmd[256];

    do {
        memset(recv_cmd, 0, 256);

        if(server->action->recv(server, (void *)recv_cmd, 256) <= 0) {
            fprintf(stderr, "server : receive command failed\n");
            sft_destroy(server);
            exit(EXIT_FAILURE);
        }

        printf("Server Reveice Command : %s\n", recv_cmd);


        if(strncmp(recv_cmd, "ls", 2) == 0 || strncmp(recv_cmd, "LS", 2) == 0) {
            //Call ls function
        }

        if(strncmp(recv_cmd, "get", 3) == 0 || strncmp(recv_cmd, "GET", 3) == 0) {
            //Call get function
        }

        if(strncmp(recv_cmd, "put", 3) == 0 || strncmp(recv_cmd, "PUT", 3) == 0) {
            //Call put function
        }

    }while(strncmp(recv_cmd, "quit", 4) != 0);

    sft_destroy(server);

    return 0;
}

