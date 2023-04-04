struct dnshdr {
    unsigned short id;           // identifier
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned char rd:1;         // recursion desired flag
    unsigned char tc:1;         // truncated message flag
    unsigned char aa:1;         // authoritative answer flag
    unsigned char opcode:4;     // operation code
    unsigned char qr:1;         // query/response flag
    unsigned char rcode:4;      // response code
    unsigned char cd:1;         // checking disabled flag
    unsigned char ad:1;         // authenticated data flag
    unsigned char z:1;          // reserved, must be zero
    unsigned char ra:1;         // recursion available flag
#elif __BYTE_ORDER == __BIG_ENDIAN
    unsigned char qr:1;         // query/response flag
    unsigned char opcode:4;     // operation code
    unsigned char aa:1;         // authoritative answer flag
    unsigned char tc:1;         // truncated message flag
    unsigned char rd:1;         // recursion desired flag
    unsigned char ra:1;         // recursion available flag
    unsigned char z:1;          // reserved, must be zero
    unsigned char ad:1;         // authenticated data flag
    unsigned char cd:1;         // checking disabled flag
    unsigned char rcode:4;      // response code
#endif
    unsigned short qdcount;      // number of questions
    unsigned short ancount;      // number of answers
    unsigned short nscount;      // number of authority records
    unsigned short arcount;      // number of additional records
};
