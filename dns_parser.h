#ifndef DNS_PARSER_H
#define DNS_PARSER_H

#include <stdio.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#define BUF_SIZE 65536
#define RECORD_NUM 10
#define NET_INTERFACE "ens33"
#define DNS_RECORD_TYPE_A 1
#define DNS_RECORD_TYPE_CNAME 5
#define CMD_SIZE 50
#define OPTION_SIZE 10

struct eth_hdr_t
{
    unsigned char h_dest[ETH_ALEN];
    unsigned char h_src[ETH_ALEN];
    unsigned short h_proto;
};  // eth_hdr

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
}; // ip_hdr

struct udp_hdr_t
{
    unsigned short src_port;
    unsigned short dest_port;
    unsigned short len;
    unsigned short checksum;
};  // udp_hdr

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
};  // dns_hdr

struct dns_question_t
{
    #if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned short qclass;
    unsigned short qtype;
    #elif __BYTE_ORDER == __BIG_ENDIAN
    unsigned short qtype;
    unsigned short qclass;
    #endif
};  // dns_question

#pragma pack(push, 1)
struct dns_resource_record_t
{
    unsigned short type;
    unsigned short class;
    unsigned int ttl;
    unsigned short length;
};  // dns_resource
#pragma pack(pop)

struct dns_record_t
{
    unsigned char *name;
    struct dns_resource_record_t *resource;
    unsigned char *rdata;
};  // dns_record

struct dns_query_t
{
    unsigned char *name;
    struct question_t *quest;
};  // dns_query

void print_buffer(unsigned char buffer[], int len);

// Find domain name length in query section
int find_qname_len(unsigned char buffer[]);

#endif