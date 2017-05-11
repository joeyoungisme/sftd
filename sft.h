#ifndef SFT_H
#define SFT_H

#include<sys/socket.h>

typedef enum {
    SFT_SERVER = 0,
    SFT_CLIENT
}SFT_TYPE;

typedef struct __SFT_DATA {
    int sockfd;

    int remotefd;
    struct sockaddr_in *info;

    struct __SFT_ACTION *action;
}SFT_DATA;


typedef struct __SFT_ACTION {
    int (*init)(struct __SFT_DATA *sft);
    int (*connect)(struct __SFT_DATA *sft, char *ipaddr, unsigned short port);
    int (*send)(struct __SFT_DATA *sft, void *data, size_t length);
    int (*recv)(struct __SFT_DATA *sft, void *data, size_t length);
    int (*info)(struct __SFT_DATA *sft);
    int (*close)(struct __SFT_DATA *sft);

}SFT_ACTION;

struct __SFT_DATA *sft_create(SFT_TYPE type);
void sft_destroy(SFT_DATA *sft);

#endif
