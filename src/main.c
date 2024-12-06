#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "defines.h"
#include "trace.h"

#define FILEBUFFS 60

uint8_t traceMode = 0;

int main(int argc, char *argv[]) {
    bool useDefF = false;
    if (argc < 2) {
        useDefF = true;
    }

    if (argc == 2) {
	// Need to check both cases now to see what we have
	if(argv[1][0] == '-') {
	    useDefF = true;
	    if(!strcmp(argv[1], "-s")) traceMode = 1; 
	}
    }
    
    if (argc > 2) {
#ifdef DEBUG
	printf("Debug mode is %s\n", (!strcmp(argv[2], "-n") ? "normal" : "silent"));
#endif
	if(!strcmp(argv[2], "-s")) traceMode = 1;
    }

    // Variables needed for file operations
    char *fileName = NULL;
    FILE *fptr = NULL;
    char buffer[FILEBUFFS];

    if (!useDefF) fileName = argv[FILELOC];
    else          fileName = "..//testing//rwims.din";

    fptr = fopen(fileName, "r");
    if (fptr == NULL) {
        printf("Error opening file %s\n", fileName);
        return 1;
    }

    while(fgets(buffer, FILEBUFFS, fptr) != NULL) {
	// Do processing for event
	if(!traceMode) {
	    printf("file read: %s", buffer);
	}
	Trace event = ParseTrace(&buffer[0]);
	// Process the buffer
    }
    // clean up allocated objects before exiting
    fclose(fptr);
    return 0;
}

