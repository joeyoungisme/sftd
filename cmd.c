#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "cmd.h"

COMMAND command_get(void)
{
    char cmd[64];

    printf("Command (help) >> ");

    fgets(cmd, 64, stdin);

    //Earse Extra Character
    if(!strchr(cmd, '\n'))
        while(fgetc(stdin) != '\n');

    if(!strncmp(cmd, "ls", 2))
        return CMD_LS;

    if(!strncmp(cmd, "get", 3))
        return CMD_GET;

    if(!strncmp(cmd, "put", 3))
        return CMD_PUT;

    if(!strncmp(cmd, "help", 4))
        return CMD_HELP;

    if(!strncmp(cmd, "quit", 4))
        return CMD_QUIT;

    return CMD_UNKNOWN;
}

int command_need_arg(COMMAND cmd)
{
    switch(cmd) {
    case CMD_GET:
    case CMD_PUT:
        return 1;

    case CMD_LS:
    case CMD_HELP:
    case CMD_QUIT:
    case CMD_UNKNOWN:
    default:
        return 0;
    }
}

int command_get_arg(char *arg, unsigned int length)
{
    if(arg == NULL || length == 0)
        return -1;

    fgets(arg, length, stdin);

    //Erase Extra Character
    if(!strchr(arg, '\n'))
        while(fgetc(stdin) != '\n');

    return 0;
}
