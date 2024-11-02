#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "defines.h"


typedef struct Traces{
	uint8_t n;
	uint32_t address;
} Trace;

void parseTrace(int lines, char buffer[lines][ADDRWIDTH+3], Trace events[lines]);

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

#ifdef DEBUG
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

#ifdef DEBUG
    for (int i = 0; i < lines; i++) {
        for (int j = 0; buffer[i][j] != '\0'; j++) printf("%c", buffer[i][j]);
        printf("\n");
    }
#endif

    fclose(fptr);

    // Parse buffer contents into struct
    Trace events[lines];
    parseTrace(lines, buffer, events);

    return 0;
}

void parseTrace(int lines, char buffer[lines][ADDRWIDTH+3], Trace events[lines]){
	int i;
	char tempAddress[ADDRWIDTH];

	for(i = 0; i < lines; i++){
		events[i].n = buffer[i][0] - '0';
		strncpy(tempAddress, &buffer[i][2], ADDRWIDTH);
		events[i].address = strtoul(tempAddress, NULL, 16);	
	
	#ifdef DEBUG
		printf("CMD = %d\taddress = %08x\n", events[i].n, events[i].address);
	#endif	
	}	
}
