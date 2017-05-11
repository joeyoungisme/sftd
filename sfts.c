#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<errno.h>
#include<netinet/in.h>


#include "pdu.h"
#include "err.h"
#include "sfts.h"

struct __SFT_ACTION *sft_server_action(void)
{
    static struct __SFT_ACTION sft_server = {
        .init = sft_server_init,
        .connect = sft_server_connect,
        .send = sft_server_send,
        .recv = sft_server_recv,
        .info = sft_server_info,
        .close = sft_server_close
    };

    return &sft_server;
}

int sft_server_init(SFT_DATA *sft)
{
    sft->sockfd = 0;

    sft->remotefd = 0;
    sft->info = NULL;

    return 0;
}

int sft_server_connect(SFT_DATA *sft, char *ipaddr, unsigned short port)
{

    struct sockaddr_in srv_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(port)
    };

    socklen_t socklen = sizeof(srv_addr);

    sft->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sft->sockfd < 0) {
        fprintf(stderr, "socket() Failed %d\n", sft->sockfd);
        return -1;
    }
    

    if(bind(sft->sockfd, (struct sockaddr *)&srv_addr, socklen) != 0) {
        fprintf(stderr, "bind() Failed\n");
        return -1;
    }

    if(listen(sft->sockfd, CONN_NUM) != 0) {
        fprintf(stderr, "listen() Failed \n");
        return -1;
    }

    sft->info = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    memset(sft->info, 0, sizeof(struct sockaddr_in));

    sft->remotefd = accept(sft->sockfd, (struct sockaddr *)sft->info, &socklen);
    if(sft->remotefd < 0) {
        fprintf(stderr, "accetp() Failed %d\n", sft->remotefd);
        return -1;
    }

    return 0;
}

int sft_server_send(SFT_DATA *sft, void *data, size_t length)
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

    return sendmsg(sft->remotefd, &hdr, 0);
}

int sft_server_recv(SFT_DATA *sft, void *data, size_t length)
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

    return recvmsg(sft->remotefd, &hdr, 0);
}

int sft_server_info(SFT_DATA *sft)
{
    printf("Show Info !!\n");
    
    return 0;
}

int sft_server_close(SFT_DATA *sft)
{
    close(sft->sockfd);
    free(sft->info);
    
    return 0;
}

