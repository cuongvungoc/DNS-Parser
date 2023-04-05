#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>

main(int argc, char *argv[])
{
   struct hostent *hp;
   struct in_addr ip_addr;

   /* Verify a "hostname" parameter was supplied */
   if (argc <1 || *argv[1] == '\0')
      exit(EXIT_FAILURE);

/* call gethostbyname() with a host name. gethostbyname() returns a */
/* pointer to a hostent struct or NULL.                             */
   hp = gethostbyname(argv[1]);

   if (!hp) {
      printf("%s was not resolved\n",argv[1]);
      exit(EXIT_FAILURE);
   }

/* move h_addr to ip_addr. This enables conversion to a form        */
/* suitable for printing with the inet_ntoa() function.             */

   ip_addr = *(struct in_addr *)(hp->h_addr);
   printf("Hostname: %s, was resolved to: %s\n",
           argv[1],inet_ntoa(ip_addr));

   exit(EXIT_SUCCESS);
}