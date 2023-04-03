import dns.message

def parse_dns_packet(packet):
    try:
        # Parse the DNS packet using dnspython
        dns_packet = dns.message.from_wire(packet)

        # Extract information from the DNS packet
        qname = str(dns_packet.question[0].name)
        qtype = dns.rdatatype.to_text(dns_packet.question[0].rdtype)
        qclass = dns.rdataclass.to_text(dns_packet.question[0].rdclass)

        # Print the extracted information
        print("DNS query for %s type %s class %s" % (qname, qtype, qclass))

    except dns.exception.DNSException as e:
        # Handle any exceptions that occur during parsing
        print("Error parsing DNS packet: %s" % e)

# Example usage: Parse a DNS packet represented as a byte string
packet = b'\x80\x01\x00\x01\x00\x01\x00\x00\x00\x00\x00\x00\x03www\x06google\x03com\x00\x00\x01\x00\x01'
parse_dns_packet(packet)
