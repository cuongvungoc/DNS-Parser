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

#define BUF_SIZE 65536

struct eth_hdr_t
{
    unsigned char h_dest[ETH_ALEN];
    unsigned char h_src[ETH_ALEN];
    unsigned short h_proto;
};

struct ip_hdr_t
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int ip_hdr_len : 4;
    unsigned int version : 4;
#elif __BYTE_ORDER == __BIG_ENDIAN
    unsigned int version : 4;
    unsigned int ip_hdr_len : 4;
#endif
    unsigned char dif_ser; // Different service
    unsigned short total_len;
    unsigned short id;
    unsigned short frag_off; // Fragment offset
    unsigned char ttl;      // Time to live
    unsigned char ip_proto; // Protocol
    unsigned short checksum;
    unsigned int scr_addr;
    unsigned int dest_addr;
};

struct udp_hdr_t
{
    unsigned short src_port;
    unsigned short dest_port;
    unsigned short len;
    unsigned short checksum;
};

struct dns_hdr_t
{
    unsigned short id; // id number
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned char rd : 1;     // recursion desired
    unsigned char tc : 1;     // truncation
    unsigned char aa : 1;     // authoritative answer
    unsigned char opcode : 4; // kind of query
    unsigned char qr : 1;     // query/response flags
    unsigned char rcode : 4;  // response code
    unsigned char cd : 1;     // checking disable flag
    unsigned char ad : 1;     // authenticated data flag
    unsigned char z : 1;      // reverse - must be zero
    unsigned char ra : 1;     // recursion available
#elif __BYTE_ORDER == __BIG_ENDIAN
    unsigned char qr : 1;     // query/response flags
    unsigned char opcode : 4; // kind of query
    unsigned char aa : 1;     // authoritative answer
    unsigned char tc : 1;     // truncation
    unsigned char rd : 1;     // recursion desired
    unsigned char ra : 1;     // recursion available
    unsigned char z : 1;      // reverse - must be zero
    unsigned char ad : 1;     // authenticated data flag
    unsigned char cd : 1;     // checking disable flag
    unsigned char rcode : 4;  // response code
#endif
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

struct dns_resource_record_t
{
    unsigned short type;
    unsigned short class;
    unsigned int ttl;
    unsigned short length;
};

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

int main()
{
    unsigned char buffer[BUF_SIZE];
    // Open a raw socket, receive
    int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0)
    {
        perror("socket");
        exit(1);
    }

    // Receive all incoming packet
    int on = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        perror("setsockopt");
        exit(1);
    }

    struct sockaddr_ll sll;
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = if_nametoindex("ens33"); // replace eth0 with your network interface
    if (bind(sockfd, (struct sockaddr *)&sll, sizeof(sll)) < 0)
    {
        perror("bind");
        exit(1);
    }

    while (1)
    {
        printf("HERE WE GO\n");
        
        memset(buffer, 0, sizeof(buffer));
        int len = recv(sockfd, buffer, sizeof(buffer), 0);
        if (len < 0)
        {
            perror("recv");
            exit(1);
        }

        for (int i = 0; i < len; i++)
        {
            printf("%.2x ", buffer[i]);
        }
        printf("\n end pkt\n.");
        printf("Ethernet header len: %ld\n", sizeof(struct eth_hdr_t));
        printf("IP header len: %ld\n", sizeof(struct ip_hdr_t));
        printf("UDP header len: %ld\n", sizeof(struct udp_hdr_t));
        printf("DNS header len: %ld\n", sizeof(struct dns_hdr_t));

        struct eth_hdr_t *eth_hdr = (struct eth_hdr_t *)&buffer;
        printf("Ether proto %d\n", ntohs(eth_hdr->h_proto));
        
        struct ip_hdr_t *ip_hdr = (struct ip_hdr_t *)&buffer[sizeof(struct eth_hdr_t)];
        printf("Ip header len: %d\n", ntohs(ip_hdr->ip_hdr_len));
        printf("Protocol: %d\n", ip_hdr->ip_proto);

        if (ip_hdr->ip_proto == 17)
        {
            struct udp_hdr_t *udp_hdr = (struct udp_hdr_t *)&buffer[sizeof(struct ip_hdr_t) + sizeof(struct eth_hdr_t)];
            printf("UDP src port: %d\n", ntohs(udp_hdr->src_port));
            if (ntohs(udp_hdr->src_port) == 53)
            {
                struct dns_hdr_t *dns_hdr = (struct dns_hdr_t *)&buffer[sizeof(struct ip_hdr_t) + sizeof(struct eth_hdr_t) + sizeof(struct udp_hdr_t)];
                printf("DNS here\n");
                
            }
        }
        printf("%d\n", len);
        // char *ip_payload = (char *)&buffer
    }
    return 0;
}