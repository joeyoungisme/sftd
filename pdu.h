#ifndef PDU_H
#define PDU_H

#define MAX_CMD_ARG_LEN     256

typedef enum {
    CMD_LS = 0,
    CMD_GET,
    CMD_PUT,

    CMD_AMOUNT
}CMD_LINE;

typedef struct {
    CMD_LINE cmd;
    char arg[MAX_CMD_ARG_LEN];
}SFTD_PDU;



#endif
