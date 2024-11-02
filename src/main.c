#include <stdio.h>
#include <stdint.h>
#include "defines.h"

int main(int argc, char *argv[]) {
    printf("Number of arguments received: %d\n", argc);

    printf("File given is %s\n", argv[FILELOC]);

    char *fileName = argv[FILELOC];

    // reading the file
    FILE *fptr = NULL;
    char ch = 0;
    uint64_t lines = 0;

    fptr = fopen(fileName, "r");
    if (fptr == NULL) {
        printf("Error opening file %s\n", fileName);
        return 1;
    }


    // count how many lines for our double buffer
    while ((ch = fgetc(fptr)) != EOF) {
        if (ch == '\n') lines++;
    }

    rewind(fptr);

    printf("Got %d lines\n", lines);

    char buffer[lines][ADDRWIDTH+2];
    uint8_t charCnt = 0;

    printf("lines again %d\n", lines);

    for (int i = 0; i < lines; i++) {
        while ((ch = fgetc(fptr)) != '\n') {
            buffer[i][charCnt] = ch;
            charCnt++;
        }
        buffer[i][charCnt] = '\0';
        charCnt = 0;
    }

    for (int i = 0; i < lines; i++) {
        for (int j = 0; buffer[i][j] != '\0'; j++) printf("%c", buffer[i][j]);
        printf("\n");
    }

    fclose(fptr);

    return 0;
}
