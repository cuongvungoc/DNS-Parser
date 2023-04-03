#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char cmd[10];
    strcpy(cmd, "dir");
    system(cmd);
    return 0;
}