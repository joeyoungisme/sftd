#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char help_message[] = 
    "Usage : sftd\n"
    "Socket File Transer Daemon\n";

void call_help(void) 
{
    printf("%s", help_message);
}
