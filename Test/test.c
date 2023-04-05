#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (strcmp(argv[1], "lock") == 0)
    {
        char cmd[20] = "ls ";
        strcat(cmd, "-l");
        // strcpy(cmd, "ls -l");

        system(cmd);
    }
    else
    {
        printf("HERE\n");
    }
    return 0;
}