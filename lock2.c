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

// int state_arlarm, 1 - state changed, 0 - not changed
int state_arlarm = 0; 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // mutex init

int main(int argc, char *argv[])
{
    pthread_t thread_ID;
    int dns_parser_thread;
    // void *retval;

    dns_parser_thread = pthread_create(&thread_ID, NULL, dns_parser, NULL);

    if (dns_parser_thread)
    {
        printf("Pthread create error number = %d\n", dns_parser_thread);
        exit(1);
    }
    // pthread_join(thread_ID, &retval);

    // Option for lock or unlock
    char cmd[CMD_SIZE] = "sh rules.sh ";
    char option[OPTION_SIZE];
    while (1)
    {
        printf("Enter your option, lock or unlock ?: ");
        scanf("%s", option);
        strcat(cmd, option);
        system(cmd);
        strcpy(cmd, "sh rules.sh ");
        // state_arlarm = 1;
        set_state_arlarm(1);
    }

    pthread_exit(NULL);

    return 0;
}