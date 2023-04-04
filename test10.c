#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 65536

void sniffer_dns_response(char *buffer, int size) {
    struct iphdr *ip_header = (struct iphdr *)buffer;
    int ip_header_length = ip_header->ihl * 4;

    struct udphdr *udp_header = (struct udphdr *)(buffer + ip_header_length);
    int udp_header_length = 8;

    // Check if the UDP packet is a DNS response message
    if (ntohs(udp_header->dest) != 53) {
        return;
    }

    char *dns_payload = buffer + ip_header_length + udp_header_length;
    int dns_payload_length = size - ip_header_length - udp_header_length;

    // Parse the DNS response message
    struct dns_header {
        unsigned short id;
        unsigned char qr :1;
        unsigned char opcode :4;
        unsigned char aa :1;
        unsigned char tc :1;
        unsigned char rd :1;
        unsigned char ra :1;
        unsigned char z :3;
        unsigned char rcode :4;
        unsigned short qdcount;
        unsigned short ancount;
        unsigned short nscount;
        unsigned short arcount;
    } __attribute__((packed));

    struct dns_header *dns_header = (struct dns_header *)dns_payload;
    unsigned short answer_count = ntohs(dns_header->ancount);

    if (answer_count == 0) {
        return;
    }

    char *answer_start = dns_payload + sizeof(struct dns_header);
    char *answer_end = dns_payload + dns_payload_length;

    while (answer_start < answer_end) {
        unsigned char label_length = *answer_start;
        if (label_length == 0) {
            answer_start++;
            continue;
        }

        if ((label_length & 0xC0) == 0xC0) {
            answer_start += 2;
            break;
        }

        answer_start += label_length + 1;
    }

    if (answer_start >= answer_end) {
        return;
    }

    struct dns_answer {
        unsigned short type;
        unsigned short class;
        unsigned int ttl;
        unsigned short rdlength;
        // The data field is variable length, so we skip it
    } __attribute__((packed));

    struct dns_answer *dns_answer = (struct dns_answer *)answer_start;
    unsigned short answer_type = ntohs(dns_answer->type);
    unsigned short answer_class = ntohs(dns_answer->class);

    if (answer_type != 1 || answer_class != 1) {
        return;
    }

    answer_start += sizeof(struct dns_answer);
    int address_length = ntohs(dns_answer->rdlength);

    if (answer_start + address_length > answer_end) {
        return;
    }

    // Print the IP address in the DNS response message
    printf("IP address: %d.%d.%d.%d\n", answer_start[0], answer_start[1], answer_start[2], answer_start[3]);
}
