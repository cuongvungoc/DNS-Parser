#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include "dns_parser.h"
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    
    char cmd[CMD_SIZE] = "sh rules.sh ";
    // Block ip from internet
    char option[OPTION_SIZE];
    printf("Enter your option, lock or unlock ?: ");
    scanf("%s", option);
    strcat(cmd, option);
    system(cmd);

    pthread_t thread_ID;
    int dns_parser_thread;

    dns_parser_thread = pthread_create(&thread_ID, NULL, dns_parser, NULL);

    if (dns_parser_thread)
    {
        printf("Pthread create error number = %d\n", dns_parser_thread);
        exit(1);
    }

    pthread_exit(NULL);

    return 0;
}