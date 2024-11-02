#include <stdio.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
    printf("Number of arguments received: %d\n", argc);

    printf("File given is %s\n", argv[1]);

    // reading the file
    FILE *fptr = NULL;
    char ch;
    uint64_t lines;

    fptr = fopen(argv[1], "r");
    if (fptr == NULL) {
        printf("Error opening file %s\n", argv[1]);
        return 1;
    }

    // count how many lines for our double buffer
    while ((ch = fgetc(fptr)) != EOF) {
        if (ch == '\n') lines++;
    }

    printf("Got %d lines\n", lines);

    return 0;
}
