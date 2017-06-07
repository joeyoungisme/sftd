#ifndef PDU_H
#define PDU_H

//#include "cmd.h"
#include "util.h"

#define MAX_CMD_LEN         256
#define MAX_CMD_ARG_LEN     256

typedef enum {
    CMD_LISTEN = 0,
    CMD_CONNECTION,
    CMD_LS,
    CMD_GET,
    CMD_PUT,
    CMD_CLOSE,
    CMD_QUIT,
    CMD_INFO,
    CMD_HELP,

    CMD_UNKNOWN
}COMMAND;

typedef struct __SFT_PDU {
    COMMAND cmd;
    char arg[MAX_CMD_ARG_LEN];
    unsigned int pdulen;
}SFT_PDU;

SFT_PDU *pdu_new(void);

int pdu_info(SFT_PDU *pdu);

int pdu_init(SFT_PDU *pdu);

int pdu_setcommand(SFT_PDU *pdu);

void pdu_free(SFT_PDU *pdu);



#endif
