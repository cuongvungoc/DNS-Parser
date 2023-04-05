#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define DNS_SERVER_IP "8.8.8.8"
#define DNS_SERVER_PORT 53
#define DNS_RECORD_TYPE_A 1

struct dns_header_t
{
    unsigned short id;        // id number
    unsigned char qr : 1;     // query/response flags
    unsigned char opcode : 4; // kind of query
    unsigned char aa : 1;     // authoritative answer
    unsigned char tc : 1;     // truncation
    unsigned char rd : 1;     // recursion desired

    unsigned char ra : 1;    // recursion available
    unsigned char z : 3;     // z! reverse
    unsigned char rcode : 4; // part of response

    unsigned short qdcount; // number of question
    unsigned short ancount; // number of answer records
    unsigned short nscount; // number of authority records
    unsigned short arcount; // number of additional records
};

struct dns_question_t
{
    unsigned short qtype;
    unsigned short qclass;
};

// #pragma pack(push, 1)
struct dns_resource_record_t
{
    unsigned short type;
    unsigned short class;
    unsigned int ttl;
    unsigned short length;
};
// #pragma pack(pop)

struct dns_record_t
{
    unsigned char *name;
    struct resource_record_t *resource;
    unsigned char *rdata;
};

struct dns_query_t
{
    unsigned char *name;
    struct question_t *quest;
};

void change_to_dns_name_format(unsigned char *dns, char *host);

void create_dns_request(char *domain_name, char *buffer);

int main(int argc, char *argv[])
{
    printf("Size of header: %ld\n", sizeof(struct dns_header_t));
    // Open a raw socket
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sockfd < 0)
    {
        perror("socket() failed!");
        exit(1);
    }

    // Enable IP_HDRINCL flag - custom IP header, not auto gen by kernel
    int option = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &option, sizeof(option)) < 0)
    {
        perror("setsockpot() failed!");
        exit(1);
    }

    // Allocate buffer for DNS request
    char buffer[BUF_SIZE];

    // memset(buffer, 0, BUF_SIZE);
    // printf("Buffer length: %ld\n", sizeof(buffer));
    // printf("BUFFER HERE\n");p
    create_dns_request("google.com", buffer);
    // printf("Buffer: %s\n length: %ld\n", buffer, strlen(buffer)); // for debug

    printf("Buffer length: %ld\n", strlen(buffer) + 1); // for debug

    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(DNS_SERVER_PORT);
    dest.sin_addr.s_addr = inet_addr(DNS_SERVER_IP);

    // Send DNS query to DNS server
    if (sendto(sockfd, buffer, sizeof(struct dns_header_t) + strlen("google.com") + 1 + sizeof(struct dns_question_t), 0, (struct sockaddr *)&dest, sizeof(dest)) < 0)
    {
        perror("Sock sendto failed!");
        exit(1);
    }

    printf("DNS request sent\n");
    // Receive DNS response from DNS server
    // struct sockaddr_in src;
    // socklen_t src_len = sizeof(src);
    int i = sizeof dest;
    // int n_byte = recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *)&dest, (socklen_t *)&i);
    if (recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *)&dest, (socklen_t *)&i) < 0)
    {
        perror("Sock recvfrom failled!");
        exit(1);
    }
    printf("Sock recvfrom success\n");
    // printf("Number of bytes receive: %d", n_byte);

    return 0;
}

void create_dns_request(char *domain_name, char *buffer)
{
    // struct dns_header_t * dns
    struct dns_header_t *dns = (struct dns_header_t *)buffer;
    // dns->id = (unsigned short)htons(getpid());
    dns->id = htons(0x00);
    // printf((unsigned short)htons(getpid()));
    dns->qr = 0;     // query
    dns->opcode = 0; // standard query
    dns->aa = 0;     // not athoritative
    dns->tc = 0;     // not truncated
    dns->rd = 1;     // recursion desired
    dns->ra = 0;     // recursion not available
    dns->z = 0;
    dns->rcode = 0;

    dns->qdcount = htons(1); // 1 question
    dns->ancount = 0;
    dns->nscount = 0;
    dns->arcount = 0;

    // Create DNS question
    unsigned char *qname = (unsigned char *)&buffer[sizeof(struct dns_header_t)];
    change_to_dns_name_format(qname, domain_name);
    int name_len = strlen(domain_name);
    // memcpy(qname, domain_name, name_len + 1);

    struct dns_question_t *qinfo = (struct dns_question_t *)&buffer[sizeof(struct dns_header_t) + name_len + 1];
    qinfo->qtype = htons(DNS_RECORD_TYPE_A); // Type A record
    qinfo->qclass = htons(1);                // Internet

    // Debug
    // printf("Number of question %d", ntohs(dns->qdcount));
}


void change_to_dns_name_format(unsigned char *dns, char *host)
{
    char *temp = malloc(strlen(host) + 1);
    temp[strlen(host)] = '.';
    for (int i = 0; i < strlen(host); i++)
    {
        temp[i] = host[i];
    }

    // char *dns = malloc(strlen(buf) + 1);
    int pos = 0;
    for (int i = 0; i < strlen(temp); i++)
    {
        if (temp[i] == '.' || temp[i] == '\0')
        {
            dns[pos] = i - pos + '0';
            for (int j = pos + 1; j <= i; j++)
            {
                dns[j] = temp[j - 1];
            }
            pos = i + 1;
        }
    }
    free(temp);
}