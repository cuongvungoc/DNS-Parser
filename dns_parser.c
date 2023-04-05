#include "dns_parser.h"

void print_buffer(unsigned char buffer[], int len)
{
    // printf("START PACKET\n");
    for (int i = 0; i < len; i++)
    {
        printf("%.2x ", buffer[i]);
    }
    printf("\n");
    // printf("\nEND PACKET\n.");
}

// Find domain name length in query section
int find_qname_len(unsigned char buffer[])
{
    int qlen = 0;
    while (buffer[qlen] < 192)
    {
        qlen++;
    }
    return qlen - 2 * sizeof(unsigned short);   // Query length - Type - Class
}