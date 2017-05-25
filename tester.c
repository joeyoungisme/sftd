#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "util.h"

int main(int argc, char *argv[]) 
{

    struct __UTILITY_DATA *util = util_create();
    
    if(!util) {
        fprintf(stderr, "util create failed !\n");
        exit(EXIT_FAILURE);
    }

    util_run(util);

    util_destroy(util);


    return 0;
}
