#include "dns_parser.h"

// state_arlarm, 1 - state changed, 0 - not changed
// int state_arlarm = 0;
// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // mutex init

int main(int argc, char *argv[])
{
    pthread_t thread_ID;
    int dns_parser_thread;

    // Option for lock or unlock
    char nslookup[CMD_SIZE] = "nslookup ";
    char cmd[CMD_SIZE] = "sh rules.sh ";
    char option[OPTION_SIZE];
    strcat(nslookup, URL);
    while (1)
    {

        // Enter optiion for run rules.sh
        printf("Enter your option, lock or unlock ?: ");
        scanf("%s", option);
        strcat(cmd, option);

        system(cmd);
        strcpy(cmd, "sh rules.sh ");

        if (0 == strcmp(option, LOCK))
        {
            dns_parser_thread = pthread_create(&thread_ID, NULL, dns_parser, NULL);
            if (dns_parser_thread)
            {
                printf("Pthread create error number = %d\n", dns_parser_thread);
                exit(1);
            }
            usleep(1000);
            // nslookup for get default IP
            system(nslookup);
                }
        else if (0 == strcmp(option, UNLOCK))
        {
            pthread_cancel(thread_ID);
            usleep(1000);
        }

        // state_arlarm = 1;
        // set_state_arlarm(1);
    }

    pthread_exit(NULL);

    return 0;
}