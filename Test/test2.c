#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct test
{
    int a;
};
#define str1 "googlecom"
int main()
{
    // char str1[] = "abc";
    // unsigned char str2[] = "googlecom";
    unsigned char *str3 = (unsigned char *)malloc(9);
    strcpy()
    printf("%d\n", strcmp(str1, (char *)str2));

    // struct test t;
    // t.a = 5;
    // printf("%d", t.a);
    // char arr[5][20];
    // strcpy(arr[0], "array0");
    // strcpy(arr[1], "array1");
    // strcpy(arr[2], "array2");

    // printf("%d", strcmp(arr[0], "array0"));
    // strcpy(arr[3], "array3");
    // strcpy(arr[4], "array4");
    // int len = sizeof(arr) / sizeof(arr[0]);
    // for (int i = 0; i < len; i++)
    // {
    //     printf("%s\n", arr[i]);
    // }
    return 0;
}