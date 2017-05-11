#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>

#include "sft.h"
#include "pdu.h"

int main(int argc, char *argv[])
{
    printf("i am client !\n");

    SFT_DATA *client = sft_create(SFT_CLIENT);
    if(client == NULL) {
        fprintf(stderr, "client sft_create failed\n");
        exit(EXIT_FAILURE);
    }

    if(0 != client->action->init(client)) {
        fprintf(stderr, "client sft_init failed\n");
        exit(EXIT_FAILURE);
    }

    if(0 != client->action->connect(client, "127.0.0.1", 9420)) {
        fprintf(stderr, "client sft_connect failed\n");
        exit(EXIT_FAILURE);
    }

    char message[] = "i am client hello world";
    int length = sizeof(message);

    sleep(1);

    if(client->action->recv(client, (void *)message, length) <= 0) {
        fprintf(stderr, "client : recv failed\n");
        sft_destroy(client);
        exit(EXIT_FAILURE);
    }

    printf("Client Recv : %s\n", message);

    snprintf(message, length, "%s", "i am client hello world");
    printf("Client Send : %s\n", message);

    if(client->action->send(client, (void *)message, length) <= 0) {
        fprintf(stderr, "client : send failed\n");
        sft_destroy(client);
        exit(EXIT_FAILURE);
    }

    sleep(10);

    sft_destroy(client);

    return 0;
}

