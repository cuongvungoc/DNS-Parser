#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char cmd[20];
    strcpy(cmd, "ifconfig");
    system(cmd);
    return 0;
}