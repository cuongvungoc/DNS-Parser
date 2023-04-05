#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#define BUF_SIZE 1024
#define DNS_SERVER_IP "8.8.8.8"
#define DNS_SERVER_PORT 53
#define DNS_QUERY_TYPE_A 1

struct dns_header {
    unsigned short id;
    unsigned short flags;
    unsigned short qdcount;
    unsigned short ancount;
    unsigned short nscount;
    unsigned short arcount;
};

struct dns_question {
    unsigned short qtype;
    unsigned short qclass;
};

void build_dns_request(char* domain_name, char* buffer) {
    // Build DNS header
    struct dns_header* dns = (struct dns_header*) buffer;
    dns->id = (unsigned short) htons(getpid());
    dns->flags = htons(0x0100); // Recursion desired
    dns->qdcount = htons(1);
    dns->ancount = 0;
    dns->nscount = 0;
    dns->arcount = 0;

    // Build DNS question
    char* qname = buffer + sizeof(struct dns_header);
    int name_len = strlen(domain_name);
    memcpy(qname, domain_name, name_len + 1);

    struct dns_question* qinfo = (struct dns_question*) (buffer + sizeof(struct dns_header) + name_len + 1);
    qinfo->qtype = htons(DNS_QUERY_TYPE_A); // Type A record
    qinfo->qclass = htons(1); // Internet class
}

int main(int argc, char* argv[]) {
    // Create raw socket
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sockfd < 0) {
        perror("socket() failed");
        exit(1);
    }

    // Enable IP_HDRINCL
    int on = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0) {
        perror("setsockopt() failed");
        exit(1);
    }

    // Allocate buffer for DNS request
    char buffer[BUF_SIZE];
    memset(buffer, 0, BUF_SIZE);

    // Build DNS request
    build_dns_request("www.example.com", buffer);

    // Send DNS request to DNS server
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(DNS_SERVER_PORT);
    dest.sin_addr.s_addr = inet_addr(DNS_SERVER_IP);
    if (sendto(sockfd, buffer, sizeof(struct dns_header) + strlen("www.example.com") + 2*sizeof(unsigned short), 0, (struct sockaddr*) &dest, sizeof(dest)) < 0) {
        perror("sendto() failed");
        exit(1);
    }

    // Receive DNS response from DNS server
    struct sockaddr_in src;
    socklen_t src_len = sizeof(src);
    int num_bytes = recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr*) &src, &src_len);
    if (num_bytes < 0) {
        perror("recvfrom() failed");
        exit(1);
    }

    // Parse DNS response and extract DNS records
    // ...

    close(sockfd);
    return 0;
}

