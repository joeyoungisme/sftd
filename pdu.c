#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "pdu.h"

SFT_PDU *pdu_new(void)
{
    SFT_PDU *p = (SFT_PDU *)malloc(sizeof(SFT_PDU));

    if(p) memset(p, 0, sizeof(SFT_PDU));

    return p;
}

int pdu_init(SFT_PDU *pdu)
{
    pdu->cmd = CMD_UNKNOWN;
    memset(pdu->arg, 0, MAX_CMD_ARG_LEN);

    return 0;
}

int pdu_setarg(SFT_PDU *pdu)
{
    switch(pdu->cmd) {
    case CMD_LS:
    case CMD_GET:
    case CMD_PUT:
        printf("Path : ");
        if(fgets(pdu->arg, MAX_CMD_ARG_LEN, stdin) <= 0)
            snprintf(pdu->arg, MAX_CMD_ARG_LEN, "%s", "./");

        return 0;
    case CMD_CONNECTION:
        printf("IP Address : ");
        if(fgets(pdu->arg, MAX_CMD_ARG_LEN, stdin) <= 0)
            return -1;
        return 0;
    default:
        return 0;
    }
}

int pdu_setcommand(SFT_PDU *pdu)
{
    printf("Command >> ");

    char command[MAX_CMD_LEN];

    if(!fgets(command, MAX_CMD_LEN, stdin)) {
        fprintf(stderr, "fgets command failed!\n");
        return -1;
    }

    if(!strchr(command, '\n'))
        while(fgetc(stdin) != '\n');

    if(strncmp(command, "ls", 2) == 0)
        pdu->cmd = CMD_LS;
    else if(strncmp(command, "get", 3) == 0)
        pdu->cmd = CMD_GET;
    else if(strncmp(command, "put", 3) == 0)
        pdu->cmd = CMD_PUT;
    else if(strncmp(command, "listen", 6) == 0)
        pdu->cmd = CMD_LISTEN;
    else if(strncmp(command, "connection", 10) == 0)
        pdu->cmd = CMD_CONNECTION;
    else if(strncmp(command, "close", 5) == 0)
        pdu->cmd = CMD_CLOSE;
    else if(strncmp(command, "quit", 4) == 0)
        pdu->cmd = CMD_QUIT;
    else if(strncmp(command, "help", 4) == 0)
        pdu->cmd = CMD_HELP;
    else
        pdu->cmd = CMD_UNKNOWN;

    pdu_setarg(pdu);

    return 0;
}


void pdu_free(SFT_PDU *pdu)
{
    if(!pdu)
        return;

    free(pdu);
}
