#include "dns_parser.h"

int is_exist(struct white_list_t *wlist, char *ip)
{
    if (0 == wlist->len)
        return 0;

    for (int i = 0; i < wlist->len; i++)
    {
        if (0 == strcmp(wlist->ip_list[i], ip))
        {
            return 1;
            break;
        }
    }
    return 0;
}

void change_to_dns_name_format(char *dns, char *host)
{
    int pos = 0;
    int len = strlen(host);
    char temp[len + 1];
    strcpy(temp, host);
    strcat(temp, ".");
    char temp2[len + 1];
    for (int i = 0; i < len + 1; i++)
    {

        if (temp[i] == '.')
        {
            temp2[pos] = i - pos + 48;
            for (int j = pos + 1; j <= i; j++)
            {
                temp2[j] = temp[j - 1];
            }
            pos = i + 1;
        }
    }
    temp2[pos] = '\0';
    strcpy(dns, temp2);
}

// Function for multithreading dns parser
void *dns_parser()
{
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
    sll.sll_ifindex = if_nametoindex(NET_INTERFACE); // Read all message from net interface
    if (bind(sockfd, (struct sockaddr *)&sll, sizeof(sll)) < 0)
    {
        perror("bind");
        exit(1);
    }

    // int sockfd = *((int *)argv);
    unsigned char buffer[BUF_SIZE];
    struct dns_record_t answer[RECORD_NUM]; // Record from dns server
    struct white_list_t *wlist = (struct white_list_t *)malloc(sizeof(struct white_list_t *));
    wlist->len = 0;
    char cmd[CMD_SIZE] = "iptables -I white-list 1 -d ";

    while (1)
    {
        // Set buffer for reading dns response message
        memset(buffer, 0, sizeof(buffer));
        int len = recv(sockfd, buffer, sizeof(buffer), 0);
        if (len < 0)
        {
            perror("recv");
            exit(1);
        }
 
        ip_header *ip_hdr = (ip_header *)&buffer[sizeof(eth_header)];

        // Check IP Protocol : 17 - UDP
        if (ip_hdr->ip_proto == 17)
        {
            // Read UDP header
            udp_header *udp_hdr = (udp_header *)&buffer[sizeof(ip_header) + sizeof(eth_header)];

            // Check UDP source port: 53 - DNS
            if (ntohs(udp_hdr->src_port) == 53)
            {
                // Read DNS header
                dns_header *dns_hdr = (dns_header *)&buffer[sizeof(ip_header) + sizeof(eth_header) + sizeof(udp_header)];

                // Read name (URL)
                unsigned char *qname = (unsigned char *)&buffer[sizeof(ip_header) + sizeof(eth_header) + sizeof(udp_header) + sizeof(dns_header)];

                // Find name length
                int qname_len = find_qname_len(qname);

                unsigned char *name = (unsigned char *)malloc(qname_len * sizeof(unsigned char));
                memcpy(name, qname, qname_len);

                /* Check name */
                char name_cmp[qname_len];
                char url_cmp[qname_len];
                // Name for compare with url
                strcpy(name_cmp, (char *)name);
                for (int i = 0; i < qname_len; i++)
                {
                    if (name_cmp[i] < 10 && name_cmp[i] != '\0')
                    {
                        name_cmp[i] += 48;
                    }
                }

                change_to_dns_name_format(url_cmp, URL);
                /* Check name */

                // If name equal to url -> get IP from dns answer
                if (0 == memcmp(name_cmp, url_cmp, qname_len))
                {
                    dns_question *dns_quest = (dns_question *)&qname[qname_len];
                    unsigned char *payload = (unsigned char *)&qname[qname_len + sizeof(dns_question)];

                    // Read Answer
                    int payload_ptr = 0;
                    for (int i = 0; i < ntohs(dns_hdr->ancount); i++)
                    {
                        answer[i].name = (unsigned char *)malloc(qname_len * sizeof(unsigned char));
                        memcpy(answer[i].name, qname, qname_len);
                        payload_ptr += sizeof(uint16_t);
                        answer[i].resource = (struct dns_resource_record_t *)&payload[payload_ptr];
                        payload_ptr += sizeof(struct dns_resource_record_t);

                        if (DNS_RECORD_TYPE_A == ntohs(answer[i].resource->type))
                        {
                            answer[i].rdata = (unsigned char *)malloc(ntohs(answer[i].resource->length) * sizeof(unsigned char));
                            for (int j = 0; j < ntohs(answer[i].resource->length); j++)
                            {
                                answer[i].rdata[j] = payload[payload_ptr + j];
                            }

                            // Convert IP to dotted decimal string
                            long *p;
                            struct sockaddr_in a;
                            p = (long *)answer[i].rdata;
                            a.sin_addr.s_addr = (*p);

                            if (0 == is_exist(wlist, inet_ntoa(a.sin_addr))) // not exist
                            {
                                // Add IP to white-list
                                strcpy(wlist->ip_list[wlist->len], inet_ntoa(a.sin_addr));
                                wlist->len += 1;

                                // Add to chain and execute rule
                                strcat(cmd, inet_ntoa(a.sin_addr));
                                strcat(cmd, " -j ACCEPT");
                                system(cmd);
                                strcpy(cmd, "iptables -I white-list 1 -d ");
                            }
                        }
                    }
                }
            }
        }
    }
}

// Find domain name length in query section
int find_qname_len(unsigned char buffer[])
{
    int qlen = 0;
    // 192 - c0 (dns pointer, point to prev name)
    while (buffer[qlen] < 192)
    {
        qlen++;
    }
    return qlen - 2 * sizeof(unsigned short); // Query length - Type - Class
}