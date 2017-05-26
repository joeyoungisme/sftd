#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>

#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include "pdu.h"
#include "err.h"
#include "sftc.h"

struct __SFT_ACTION *sft_client_action(void)
{
    static struct __SFT_ACTION sft_client = {
        .init = sft_client_init,
        .connect = sft_client_connect,
        .send = sft_client_send,
        .recv = sft_client_recv,
        .info = sft_client_info,
        .close = sft_client_close
    };

   return &sft_client;
}

int sft_client_init(SFT_DATA *sft)
{
    sft->type = SFT_CLIENT;
    sft->sockfd = 0;

    sft->remotefd = 0;
    sft->info = NULL;

    return 0;
}

int sft_client_connect(SFT_DATA *sft, char *ipaddr, unsigned short port)
{
    const struct sockaddr_in remote_info = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(ipaddr),
        .sin_port = htons(port)
    };

    socklen_t socklen = sizeof(remote_info);

    sft->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sft->sockfd < 0) {
        fprintf(stderr, "socket() Failed %d\n", sft->sockfd);
        return -1;
    }

    if(connect(sft->sockfd, (struct sockaddr *)&remote_info, socklen) < 0) {
        fprintf(stderr, "connect() Failed \n");
        return -1;
    }

    sft->info = &remote_info;

    return 0;
}

int sft_client_send(SFT_DATA *sft, void *data, size_t length)
{
    struct iovec iov = {
        .iov_base = data,
        .iov_len = length
    };

    struct msghdr hdr;
    memset(&hdr, 0, sizeof(struct msghdr));

    hdr.msg_iov = &iov;
    hdr.msg_iovlen = 1;
    hdr.msg_control = NULL;
    hdr.msg_controllen = 0;

    return sendmsg(sft->sockfd, &hdr, 0);
}

int sft_client_recv(SFT_DATA *sft, void *data, size_t length)
{
    struct iovec iov = {
        .iov_base = data,
        .iov_len = length
    };

    struct msghdr hdr;
    memset(&hdr, 0, sizeof(struct msghdr));

    hdr.msg_iov = &iov;
    hdr.msg_iovlen = 1;
    hdr.msg_control = NULL;
    hdr.msg_controllen = 0;

    return recvmsg(sft->sockfd, &hdr, 0);
}

int sft_client_info(SFT_DATA *sft)
{
    if(!sft)
        return -1;

    printf("SFT : \n");

    if(sft->type == SFT_CLIENT)
        printf("\tType : Client\n");
    else
        printf("\tType : Server\n");

    printf("\tSocket fd : %d\n", sft->sockfd);

    if(sft->type == SFT_SERVER)
        printf("\tRemote fd : %d\n", sft->remotefd);

    return 0;
}

int sft_client_close(SFT_DATA *sft)
{
    close(sft->sockfd);

    return 0;
}

