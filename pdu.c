#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "pdu.h"

SFT_PDU *pdu_new(void)
{
    SFT_PDU *p = (SFT_PDU *)malloc(sizeof(SFT_PDU));
    memset(p, 0, sizeof(MAX_CMD_ARG_LEN));
    return p;
}


int pdu_set(SFT_PDU *pdu)
{
    pdu->cmd = command_get();

    if(command_need_arg(pdu->cmd)) {
        printf("file name : ");
        command_get_arg(pdu->arg, MAX_CMD_ARG_LEN);
    }

    return 0;
}


void pdu_free(SFT_PDU *pdu)
{
    if(pdu == NULL)
        return;

    free(pdu);
}
