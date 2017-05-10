#ifndef PDU_H
#define PDU_H

#include "cmd.h"

#define MAX_CMD_ARG_LEN     256

typedef struct {
    COMMAND cmd;
    char arg[MAX_CMD_ARG_LEN];
}SFT_PDU;

SFT_PDU *pdu_new(void);

int pdu_set(SFT_PDU *pdu);

void pdu_free(SFT_PDU *pdu);



#endif
