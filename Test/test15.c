#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>

int main() {
    char *url = "www.google.com";
    struct hostent *he;
    struct in_addr **addr_list;

    if ((he = gethostbyname(url)) == NULL) {
        printf("Cannot get host by name\n");
        return 1;
    }

    addr_list = (struct in_addr **) he->h_addr_list;

    printf("IP address of %s:\n", url);

    for (int i = 0; addr_list[i] != NULL; i++) {
        printf("%s\n", inet_ntoa(*addr_list[i]));
    }

    return 0;
}
