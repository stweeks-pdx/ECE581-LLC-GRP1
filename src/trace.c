#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "trace.h"

Trace* parseTrace(int lines, char buffer[lines][ADDRWIDTH+3]);
    
Trace* ParseTraceFile(char *fileName, uint64_t* eSize) {
    FILE *fptr = NULL;
    char ch = 0;
    uint64_t lines = 0;

    fptr = fopen(fileName, "r");
    if (fptr == NULL) {
        printf("Error opening file %s\n", fileName);
        return NULL;
    }


    // count how many lines for our double buffer
    while ((ch = fgetc(fptr)) != EOF) {
        if (ch == '\n') lines++;
    }

    // Need to reset head pointer after counting sizes
    rewind(fptr);

#ifdef DEBUG
    printf("Got %ld lines\n", lines);
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

    // Parse buffer contents into struct and return pointer and size
    Trace* events = parseTrace(lines, buffer);

    *eSize = lines;
    return events;
}

Trace* parseTrace(int lines, char buffer[lines][ADDRWIDTH+3]) {
	int i;
	char tempAddress[ADDRWIDTH];

	Trace *events = (Trace*)malloc(lines * sizeof(Trace));
	if (events == NULL) {
#ifdef DEBUG
		printf("Malloc for events returned a NULL pointer\n");
#endif
		return NULL;
	}

	for(i = 0; i < lines; i++){
		events[i].n = buffer[i][0] - '0';
		strncpy(tempAddress, &buffer[i][2], ADDRWIDTH);
		events[i].address = strtoul(tempAddress, NULL, 16);	
	
#ifdef DEBUG
		printf("CMD = %d\taddress = %08x\n", events[i].n, events[i].address);
#endif	
	}

	return events;
}

