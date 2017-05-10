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
#include "sftd.h"

int main(int argc, char *argv[])
{
    int res = 0;

    fprintf(stderr, "sftd start...\n");

    struct sockaddr_in srv_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(SFTD_PORT)
    };

    socklen_t socklen = sizeof(srv_addr);

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd < 0) {
        fprintf(stderr, "socket() Failed %d\n", sock_fd);
        exit(EXIT_FAILURE);
    }

    res = bind(sock_fd, (struct sockaddr *)&srv_addr, socklen);
    if(res != 0) {
        fprintf(stderr, "bind() Failed %d\n", res);
        exit(EXIT_FAILURE);
    }

    res = listen(sock_fd, CONN_NUM);
    if(res != 0) {
        fprintf(stderr, "listen() Failed %d\n", res);
        exit(EXIT_FAILURE);
    }

    int remote_fd = accept(sock_fd, (struct sockaddr *)&srv_addr, &socklen);
    if(remote_fd < 0) {
        fprintf(stderr, "accetp() Failed %d\n", remote_fd);
        exit(EXIT_FAILURE);
    }

    SFT_PDU *pdu = pdu_new();

    struct iovec iov = {
        .iov_base = (void *)pdu,
        .iov_len = sizeof(SFT_PDU)
    };

    struct msghdr hdr;
    memset(&hdr, 0, sizeof(struct msghdr));

    hdr.msg_iov = &iov;
    hdr.msg_iovlen = 1;
    hdr.msg_control = NULL;
    hdr.msg_controllen = 0;

    printf("Recvice Res : %d\n", (int)recvmsg(remote_fd, &hdr, 0));

    printf("cmd = %d, Argument = %s\n", (int)pdu->cmd, pdu->arg);

    pdu_free(pdu);
    close(sock_fd);

    return 0;

}
