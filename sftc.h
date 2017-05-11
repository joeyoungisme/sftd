#ifndef SFTC_H
#define SFTC_H

#include "sft.h"

struct __SFT_ACTION *sft_client_action(void);


int sft_client_init(SFT_DATA *sft);
int sft_client_connect(SFT_DATA *sft, char *ipaddr, unsigned short port);
int sft_client_send(SFT_DATA *sft, void *data, size_t length);
int sft_client_recv(SFT_DATA *sft, void *data, size_t length);
int sft_client_info(SFT_DATA *sft);
int sft_client_close(SFT_DATA *sft);



#endif
