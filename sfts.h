#ifndef SFTS_H
#define SFTS_H

#define SFTD_PORT   9420
#define CONN_NUM    10

#include "sft.h"

struct __SFT_ACTION *sft_server_action(void);

int sft_server_init(SFT_DATA *sft);
int sft_server_connect(SFT_DATA *sft, char *ipaddr, unsigned short port);
int sft_server_send(SFT_DATA *sft, void *data, size_t length);
int sft_server_recv(SFT_DATA *sft, void *data, size_t length);
int sft_server_info(SFT_DATA *sft);
int sft_server_close(SFT_DATA *sft);

#endif
