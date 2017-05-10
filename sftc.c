#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>

#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include "pdu.h"
#include "err.h"
#include "sftc.h"
#include "sftd.h"

int main(int argc, char *argv[])
{
    //Get IP or Domain
    if(argc < 2) {
        call_help();
        exit(EXIT_FAILURE);
    }

    REMOTE_INFO remote = {
        .sockfd = 0,
        .ipaddr = "",
        .domain = ""
    };

    int opt = 0;
    while((opt = getopt(argc, argv, "i:d:")) != -1) {
        switch(opt) {
        case 'i':
            snprintf(remote.ipaddr, 16, "%s", optarg);
            break;
        case 'd':
            snprintf(remote.domain, 128, "%s", optarg);
            break;
        default:
            call_help();
            exit(EXIT_FAILURE);
        }
    }

    struct sockaddr_in remote_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(remote.ipaddr),
        .sin_port = htons(SFTD_PORT)
    };

    socklen_t socklen = sizeof(remote_addr);

    int res = 0;

    remote.sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(remote.sockfd < 0) {
        fprintf(stderr, "socket() Failed %d\n", remote.sockfd);
        exit(EXIT_FAILURE);
    }

    res = connect(remote.sockfd, (struct sockaddr *)&remote_addr, socklen);
    if(res < 0) {
        fprintf(stderr, "connect() Failed %d\n", res);
        exit(EXIT_FAILURE);
    }

    printf("Connect Complete!!\n");
    sleep(3);
    printf("Close Socket!\n");

    close(remote.sockfd);

    return 0;

}
