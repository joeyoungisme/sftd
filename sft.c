#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include "sft.h"
#include "sfts.h"
#include "sftc.h"

struct __SFT_DATA *sft_create(SFT_TYPE type)
{
    const SFT_ACTION *action = NULL;

    switch(type) {
    case SFT_SERVER :
        action = sft_server_action();
        break;
    case SFT_CLIENT :
        action = sft_client_action();
        break;
    default :
        return NULL;
    }

    if(!action) {
        return NULL;
    }

    SFT_DATA *sft = (SFT_DATA *)malloc(sizeof(SFT_DATA));
    if(sft) {
        memset(sft, 0, sizeof(SFT_DATA));
        sft->action = action;
    }
    return sft;
}

void sft_destroy(SFT_DATA *sft)
{
    sft->action->close(sft);
    free(sft);
}
