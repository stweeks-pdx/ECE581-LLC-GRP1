#include <stdio.h>
#include <stdint.h>
#include "defines.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Not enough args given to run this program\n");
	return 1;
    }

    // Variables needed for file operations
    char *fileName = argv[FILELOC];
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

#if DEBUG
    printf("Got %d lines\n", lines);
#endif

    char buffer[lines][ADDRWIDTH+3];
    uint8_t charCnt = 0;

    for (int i = 0; i < lines; i++) {
        while ((ch = fgetc(fptr)) != '\n') {
            buffer[i][charCnt] = ch;
            charCnt++;
        }
        buffer[i][charCnt] = '\0';
        charCnt = 0;
    }

#if DEBUG
    for (int i = 0; i < lines; i++) {
        for (int j = 0; buffer[i][j] != '\0'; j++) printf("%c", buffer[i][j]);
        printf("\n");
    }
#endif

    fclose(fptr);

    return 0;
}
