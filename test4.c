#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <libnet.h>

#define SIZE_ETHERNET 14
#define SIZE_UDP 8

void handle_dns_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

int main(int argc, char *argv[]) {

    char *dev = NULL;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    struct bpf_program fp;
    bpf_u_int32 net;
    bpf_u_int32 mask;

    // Get default network device
    dev = pcap_lookupdev(errbuf);
    if (dev == NULL) {
        fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
        return 2;
    }

    // Open network device in promiscuous mode
    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        return 2;
    }

    // Get network address and netmask of the network device
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Couldn't get netmask for device %s: %s\n", dev, errbuf);
        net = 0;
        mask = 0;
    }

    // Compile and apply BPF filter to capture only DNS packets
    if (pcap_compile(handle, &fp, "udp port 53", 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter: %s\n", pcap_geterr(handle));
        return 2;
    }
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter: %s\n", pcap_geterr(handle));
        return 2;
    }

    // Start capturing packets
    pcap_loop(handle, -1, handle_dns_packet, NULL);

    // Close the capture device
    pcap_close(handle);

    return 0;
}

void handle_dns_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {

    const struct ether_header *ethernet;
    const struct ip *ip;
    const struct udphdr *udp;
    const u_char *payload;

    // Get Ethernet header
    ethernet = (struct ether_header*)packet;

    // Get IP header
    ip = (struct ip*)(packet + SIZE_ETHERNET);

    // Get UDP header
    udp = (struct udphdr*)(packet + SIZE_ETHERNET + sizeof(struct ip));

    // Get payload (DNS packet)
    payload = (u_char*)(packet + SIZE_ETHERNET + sizeof(struct ip) + SIZE_UDP);

    libnet_t *l;
    libnet_ptag_t ptag;
    l = libnet_init(LIBNET_RAW4, NULL, NULL, NULL);
    ptag = libnet_build_dnsv4(
        ntohs(udp->uh_ulen) - SIZE_UDP,
        payload,
        ntohs(ip->ip_len) - sizeof(struct ip),
        0,
        1,
        0,
       
