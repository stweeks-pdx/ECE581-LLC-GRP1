#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "defines.h"
#include "trace.h"

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
    if (!useDefF) fileName = argv[FILELOC];
    else          fileName = "..//testing//rwims.din";

    // create events buffer an populate it with size returned
    uint64_t eSize = 0;
    Trace* events = ParseTraceFile(fileName, &eSize);
    
    if (events == NULL) {
        printf("Received an empty events buffer, exiting with failure\n");
	return 1;
    }

    // Do processing for events

    // clean up allocated objects before exiting
    free(events);
    return 0;
}

