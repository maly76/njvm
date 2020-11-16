#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
int main(int argc, char *argv[])
{
    printf("Ninja Virtual Machine started\n the arguments are:\n");

    for (int j = 0; j < argc; j++)
    {
        printf("%s\n", argv[j]);
    }

    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], "--help") == 0)
        {
            printf("usage: ./njvm [option] [option] ...\n"
                   "--version        show version and exit\n"
                   "--help           show this help and exit\n");
        }
        else if(strcmp(argv[i], "--version") == 0)
        {
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            printf("Ninja Virtual Machine version 0 (compiled %d-%02d-%02d %02d:%02d:%02d\n",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,tm.tm_min,tm.tm_sec);
        }
        else
        {
            printf("unknown command line argument '%s', try './njvm --help'\n", argv[i]);
        }
    }
    printf("Ninja Virtual Machine stopped\n");
    return 0;
}