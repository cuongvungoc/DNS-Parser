static void *dns_parser(void *thread_args)
{
    struct argv_t *argv = (struct argv_t *)thread_args;
    unsigned char buffer[BUF_SIZE];
    struct dns_record_t answer[RECORD_NUM]; // Record from dns server
    while (1)
    {
        printf("\n====================== INCOMING MESSAGE ======================\n");
        memset(buffer, 0, sizeof(buffer));
        int len = recv(sockfd, buffer, sizeof(buffer), 0);
        if (len < 0)
        {
            perror("recv");
            exit(1);
        }

        struct eth_hdr_t *eth_hdr = (struct eth_hdr_t *)&buffer;
        printf("Ethernet protocol: %d\n", ntohs(eth_hdr->h_proto));

        struct ip_hdr_t *ip_hdr = (struct ip_hdr_t *)&buffer[sizeof(struct eth_hdr_t)];
        printf("IP protocol: %d\n", ip_hdr->ip_proto);

        // Check IP Protocol : 17 - UDP
        if (ip_hdr->ip_proto == 17)
        {
            struct udp_hdr_t *udp_hdr = (struct udp_hdr_t *)&buffer[sizeof(struct ip_hdr_t) + sizeof(struct eth_hdr_t)];
            // printf("UDP src port: %d\n", ntohs(udp_hdr->src_port));
            // Check UDP source port: 53 - DNS
            if (ntohs(udp_hdr->src_port) == 53)
            {
                print_buffer(buffer, len);
                struct dns_hdr_t *dns_hdr = (struct dns_hdr_t *)&buffer[sizeof(struct ip_hdr_t) + sizeof(struct eth_hdr_t) + sizeof(struct udp_hdr_t)];
                printf("Number of Question: %d\n", ntohs(dns_hdr->qdcount));
                printf("Number of Answer: %d\n", ntohs(dns_hdr->ancount));
                printf("Number of Authority: %d\n", ntohs(dns_hdr->nscount));
                printf("Number of Additional: %d\n", ntohs(dns_hdr->arcount));
                unsigned char *qname = (unsigned char *)&buffer[sizeof(struct ip_hdr_t) + sizeof(struct eth_hdr_t) + sizeof(struct udp_hdr_t) + sizeof(struct dns_hdr_t)];

                int qname_len = find_qname_len(qname);
                printf("Domain name length: %d\n", qname_len);
                unsigned char *name = (unsigned char *)malloc(qname_len * sizeof(unsigned char));
                memcpy(name, qname, qname_len);
                printf("Name: %s\n", name);

                struct dns_question_t *dns_quest = (struct dns_question_t *)&qname[qname_len];
                printf("Type: %d, Class: %d\n", ntohs(dns_quest->qclass), ntohs(dns_quest->qtype));

                unsigned char *payload = (unsigned char *)&qname[qname_len + sizeof(struct dns_question_t)];
                print_buffer(payload, 20);
                
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
                            printf("%.2x ", answer[i].rdata[j]);
                        }
                        printf("\n");

                        // Convert IP to dotted decimal string
                        long *p;
                        struct sockaddr_in a;
                        p = (long *)answer[i].rdata;
                        a.sin_addr.s_addr = (*p);
                        printf("IP: %s\n", inet_ntoa(a.sin_addr));
                    }
                }
            }
        }
    }
}