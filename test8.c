#include <stdio.h>

int main() {
    char my_array[] = "\0Hello\0World!\0";
    int length = 0;
    int i;

    // Loop through the array and count the number of non-null characters
    for (i = 0; i < sizeof(my_array); i++) {
        if (my_array[i] != '\0') {
            length++;
        }
    }

    printf("Length of array: %d\n", length);

    return 0;
}
