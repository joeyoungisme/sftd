#ifndef CMD_H
#define CMD_H

typedef enum {
    CMD_LS = 0, 
    CMD_GET,
    CMD_PUT,
    CMD_QUIT,
    CMD_HELP,

    CMD_UNKNOWN
}COMMAND;

COMMAND command_get(void);

//if need arg return 1, if not need arg return 0
int command_need_arg(COMMAND cmd);

int command_get_arg(char *arg, unsigned int length);

#endif
