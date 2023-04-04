#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 65536

struct dnshdr {
    unsigned short id;
    unsigned char rd :1;
    unsigned char tc :1;
    unsigned char aa :1;
    unsigned char opcode :4;
    unsigned char qr :1;
    unsigned char rcode :4;
    unsigned char cd :1;
    unsigned char ad :1;
    unsigned char z :1;
    unsigned char ra :1;
    unsigned short q_count;
    unsigned short ans_count;
    unsigned short auth_count;
    unsigned short add_count;
};

void print_dns_query(char* buf, int size) {
    struct dnshdr *dns = (struct dnshdr*) buf;
    unsigned char *qname = (unsigned char*)&buf[sizeof(struct dnshdr)];

    printf("DNS Query Request:\n");

    // print the DNS header fields
    printf("ID: %d\n", ntohs(dns->id));
    printf("QR: %d\n", dns->qr);
    printf("Opcode: %d\n", dns->opcode);
    printf("AA: %d\n", dns->aa);
    printf("TC: %d\n", dns->tc);
    printf("RD: %d\n", dns->rd);
    printf("RA: %d\n", dns->ra);
    printf("Z: %d\n", dns->z);
    printf("RCode: %d\n", dns->rcode);
    printf("QCount: %d\n", ntohs(dns->q_count));
    printf("AnsCount: %d\n", ntohs(dns->ans_count));
    printf("AuthCount: %d\n", ntohs(dns->auth_count));
    printf("AddCount: %d\n", ntohs(dns->add_count));

    // print the query name
    printf("Query Name: ");
    int i = 0;
    while (qname[i] != 0) {
        printf("%c", qname[i]);
        i++;
    }
    printf("\n");
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

#define MAX_BUFFER_SIZE 65536

int main(int argc, char *argv[]) {
    int raw_socket;
    char buffer[MAX_BUFFER_SIZE];

    // Create a raw socket
    raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (raw_socket < 0) {
        perror("Failed to create raw socket");
        exit(EXIT_FAILURE);
    }

    // Set the socket option to receive all incoming packets
    int on = 1;
    if (setsockopt(raw_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("Failed to set socket option");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the network interface
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(53);
    server_address.sin_addr.s_addr = INADDR_ANY;
    if (bind(raw_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }

    // Sniff DNS response messages
    while (1) {
        int length = recvfrom(raw_socket, buffer, MAX_BUFFER_SIZE, 0, NULL, NULL);
        if (length < 0) {
            perror("Failed to receive data");
            exit(EXIT_FAILURE);
        }
        printf("%d", length);
        // Parse the DNS response message
        // sniffer_dns_response(buffer, length);
    }

    return 0;
}
