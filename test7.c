#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    char buf[20];
    scanf("%s", buf);
    buf[strlen(buf)] = '.';

    char *dns = malloc(strlen(buf) + 1);
    int pos = 0;
    for (int i = 0; i < strlen(buf); i++)
    {
        if (buf[i] == '.' || buf[i] == '\0')
        {
            dns[pos] = i - pos + '0';
            for (int j = pos + 1; j <= i; j++)
            {
                dns[j] = buf[j - 1];
            }
            pos = i + 1;
        }
    }
    printf("%s\n", dns);
    return 0;
}