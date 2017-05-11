#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>

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

    char message[] = "hello world i am server";
    int length = sizeof(message);

    printf("Server Send : %s\n", message);

    if(server->action->send(server, (void *)message, length) <= 0) {
        fprintf(stderr, "server : send failed \n");
        sft_destroy(server);
        exit(EXIT_FAILURE);
    }

    sleep(2);

    if(server->action->recv(server, (void *)message, length) <= 0) {
        fprintf(stderr, "server : recv failed\n");
        sft_destroy(server);
        exit(EXIT_FAILURE);
    }

    printf("Server Recv : %s\n", message);

    sleep(10);

    sft_destroy(server);

    return 0;
}

