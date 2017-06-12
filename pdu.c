#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "pdu.h"

static char remote_addr[MAX_PATH_LEN] = "./";
static char local_addr[MAX_PATH_LEN] = "./";

static char remote_temp[MAX_PATH_LEN] = "./";
static char local_temp[MAX_PATH_LEN] = "./";

static char remotefile[MAX_PATH_LEN] = "";
static char localfile[MAX_PATH_LEN] = "";

static void setaddr(char *base, char *addr)
{
    char temp[MAX_PATH_LEN];

    if(strncmp(addr, "/", 1) == 0)
        strncpy(base, addr, MAX_PATH_LEN);
    else {
        if(strncmp(addr, "./", 2) == 0)
            snprintf(temp, MAX_PATH_LEN, "%s/%s", base, addr+2);
        else
            snprintf(temp, MAX_PATH_LEN, "%s/%s", base, addr);

        strncpy(base, temp, MAX_PATH_LEN);
    }
}

static void setfilename(char *base, char *name)
{
    strncpy(base, name, MAX_PATH_LEN);
}

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
    pdu->pdulen = sizeof(struct __SFT_PDU);

    return 0;
}

char *pdu_remoteaddr(void)
{
    return remote_temp;
}

char *pdu_localaddr(void)
{
    return local_temp;
}

char *pdu_remotefile(void)
{
    return remotefile;
}

char *pdu_localfile(void)
{
    return localfile;
}

void pdu_saveaddr(void)
{
    strncpy(remote_addr, remote_temp, MAX_PATH_LEN);
    strncpy(local_addr, local_temp, MAX_PATH_LEN);
}

void pdu_recoveryaddr(void)
{
    strncpy(remote_temp, remote_addr, MAX_PATH_LEN);
    strncpy(local_temp, local_addr, MAX_PATH_LEN);
}

int pdu_setargument(SFT_PDU *pdu, char *arg)
{
    if(arg)
        if(!strncpy(pdu->arg, arg, MAX_CMD_ARG_LEN))
            return -1;
    return 0;
}

int pdu_setcommand(SFT_PDU *pdu, char *userinput)
{
    pdu_init(pdu);

    if(strncmp(userinput, "ls", 2) == 0)
        pdu->cmd = CMD_LS;
    else if(strncmp(userinput, "get", 3) == 0)
        pdu->cmd = CMD_GET;
    else if(strncmp(userinput, "put", 3) == 0)
        pdu->cmd = CMD_PUT;
    else if(strncmp(userinput, "connection", 10) == 0)
        pdu->cmd = CMD_CONNECTION;
    else if(strncmp(userinput, "listen", 6) == 0)
        pdu->cmd = CMD_LISTEN;
    else if(strncmp(userinput, "close", 5) == 0)
        pdu->cmd = CMD_CLOSE;
    else if(strncmp(userinput, "quit", 4) == 0)
        pdu->cmd = CMD_QUIT;
    else if(strncmp(userinput, "help", 4) == 0)
        pdu->cmd = CMD_HELP;
    else if(strncmp(userinput, "info", 4) == 0)
        pdu->cmd = CMD_INFO;
    else
        pdu->cmd = CMD_UNKNOWN;

    char command[MAX_CMD_LEN];
    char local[MAX_CMD_LEN];
    char remote[MAX_CMD_LEN];
    int count = 0;

    switch(pdu->cmd) {
    case CMD_LS:
        count = sscanf(userinput, "%s %s", command, remote);
        if(count == 1)
            return 0;
        else if(count != 2) {
            fprintf(stderr, "Command Error : Argument Error!\n");
            return -1;
        }
        setaddr(remote_temp, remote);
        return 0;
    case CMD_GET:
    case CMD_PUT:
        count = sscanf(userinput, "%s %s %s", command, local, remote);
        if(count != 3) {
            fprintf(stderr, "Command Error : Argument Error!\n");
            return -1;
        }
        setfilename(localfile, local);
        setfilename(remotefile, remote);
        *strrchr(local, '/') = '\0';
        *strrchr(remote, '/') = '\0';
        setaddr(local_temp, local);
        setaddr(remote_temp, remote);
        return 0;
    case CMD_CONNECTION:
        count = sscanf(userinput, "%s %s", command, pdu->arg);
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

void pdu_error(SFT_PDU *pdu, const char *message)
{
    pdu_init(pdu);
    pdu->cmd = CMD_ERROR;
    snprintf(pdu->arg, MAX_CMD_ARG_LEN, message);
}

void pdu_free(SFT_PDU *pdu)
{
    if(!pdu)
        return;

    free(pdu);
}
