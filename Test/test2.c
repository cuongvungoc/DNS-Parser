#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char str[10];
    scanf("%s", str);
    if (0 == strcmp(str, "0"))
    {
        printf("Space");
    }
    return 0;
}