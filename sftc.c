#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>

#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>

#include "pdu.h"
#include "err.h"

int main(int argc, char *argv[])
{
    //Get IP or Domain
    if(argc < 2) {
        call_help();
        exit(EXIT_FAILURE);
    }



    return 0;

}
