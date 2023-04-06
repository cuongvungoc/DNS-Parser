#include <stdio.h>
#include <string.h>

#define url "google.com"
void change_to_dns_name_format(char *dns, char *host)
{
    int pos = 0;
    int len = strlen(host);
    char temp[len + 1];
    strcpy(temp, host);
    strcat(temp, ".");
    // strcpy(dns, temp);
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

int main()
{
    char dns[11];
    change_to_dns_name_format(dns, url);
    printf("%s\n", dns);
    return 0;
}
