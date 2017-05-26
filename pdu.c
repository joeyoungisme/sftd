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

int pdu_info(SFT_PDU *pdu)
{
    if(!pdu)
        return -1;

    printf("PDU : \n");

    printf("\tCommand : ");
    switch(pdu->cmd) {
    case CMD_LISTEN:
        printf("Listen\n");
        break;
    case CMD_CONNECTION:
        printf("Connection\n");
        break;
    case CMD_LS:
        printf("Ls\n");
        break;
    case CMD_GET:
        printf("Get\n");
        break;
    case CMD_PUT:
        printf("Put\n");
        break;
    case CMD_CLOSE:
        printf("Close\n");
        break;
    case CMD_QUIT:
        printf("Quit\n");
        break;
    case CMD_INFO:
        printf("Info\n");
        break;
    case CMD_HELP:
        printf("Help\n");
        break;
    default:
        return 0;
    }

    printf("\tArgument : %s\n", pdu->arg);

    return 0;
}

    

int pdu_init(SFT_PDU *pdu)
{
    pdu->cmd = CMD_UNKNOWN;
    memset(pdu->arg, 0, MAX_CMD_ARG_LEN);

    return 0;
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
    else if(strncmp(command, "connection", 10) == 0)
        pdu->cmd = CMD_CONNECTION;
    else if(strncmp(command, "listen", 6) == 0)
        pdu->cmd = CMD_LISTEN;
    else if(strncmp(command, "close", 5) == 0)
        pdu->cmd = CMD_CLOSE;
    else if(strncmp(command, "quit", 4) == 0)
        pdu->cmd = CMD_QUIT;
    else if(strncmp(command, "help", 4) == 0)
        pdu->cmd = CMD_HELP;
    else if(strncmp(command, "info", 4) == 0)
        pdu->cmd = CMD_INFO;
    else
        pdu->cmd = CMD_UNKNOWN;

    int count = 0;
    char cmd[MAX_CMD_LEN];

    switch(pdu->cmd) {
    case CMD_LS:
        count = sscanf(command, "%s %s", cmd, pdu->arg);

        if(count == 1)
            snprintf(pdu->arg, MAX_CMD_ARG_LEN, "%s", "./");
        else if(count != 2) {
            fprintf(stderr, "Command Error : Argument Error!\n");
            return -1;
        }

        return 0;
    case CMD_GET:
    case CMD_PUT:
        count = sscanf(command, "%s %s", cmd, pdu->arg);
        if(count != 2) {
            fprintf(stderr, "Command Error : Argument Error!\n");
            return -1;
        }
        return 0;
    case CMD_CONNECTION:
        count = sscanf(command, "%s %s", cmd, pdu->arg);
        if(count == 1)
            snprintf(pdu->arg, MAX_CMD_ARG_LEN, "%s", "127.0.0.1");
        else if(count != 2) {
            fprintf(stderr, "Command Error : Argument Error!\n");
            return -1;
        }

        unsigned int ip[4];
        if(sscanf(pdu->arg, "%u.%u.%u.%u", ip, ip+1, ip+2, ip+3) != 4) {
            fprintf(stderr, "Command Error : Argument Error!\n");
            return -1;
        }

        for(int index = 0; index < 4; ++index) {
            if(ip[index] > 255) {
                fprintf(stderr, "Command Error : Argument Error!\n");
                return -1;
            }
        }
        return 0;
    default:
        return 0;
    }

    return 0;
}


void pdu_free(SFT_PDU *pdu)
{
    if(!pdu)
        return;

    free(pdu);
}
