#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char help_message[] =
    "Usage : %s [option]\n"
    " -i < ip address >             remote ip address , Ex : 192.168.1.1\n"
    " -d < domain name >            remote domain , Ex : www.google.com\n";

void call_help(char *name)
{
    printf(help_message, name);
}
