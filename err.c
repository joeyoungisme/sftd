#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char help_message[] = 
    "Usage : sftc [option]\n"
    " -i : remote ip address , Ex : 192.168.1.1\n"
    " -d : remote domain , Ex : www.google.com\n";

void call_help(void) 
{
    printf("%s", help_message);
}
