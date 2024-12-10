#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <getopt.h>
#include <math.h>

#include "defines.h"
#include "trace.h"
#include "cache.h"

// Global variables from our arguments
uint32_t CAPACITY = 24;
uint32_t ASSOCIATIVITY = 16;
uint32_t BYTESELECTWIDTH = 6;
char *fileName = "..//testing//rwims.din";

// Global variables set by arguments
uint8_t normalMode = 1;
uint32_t BYTESELECTMASK = 0x3F;
uint32_t INDEXWIDTH = 14;
uint32_t INDEXMASK = 0x3FFF;
uint32_t TAGWIDTH = 12;
uint32_t TAGMASK = 0xFFF;
uint32_t SETS = 16384;

// makeMask(maskWidth) creates & mask based on width
uint32_t makeMask(uint32_t maskWidth);

int main(int argc, char *argv[]) {
    int c;
    
    // Parse arguments
    while((c = getopt(argc, argv, "sc:a:l:f:h")) != -1){
        switch(c){
            case 's':
                normalMode = 0;
                break;
            case 'c':
                CAPACITY = atoi(optarg);
                break;
            case 'a':
                ASSOCIATIVITY = atoi(optarg);
                break;
            case 'l': 
                BYTESELECTWIDTH = atoi(optarg);
                break;
            case 'f':
                fileName = optarg;
                break;
            case 'h':
		printf("Options for the program are:\n");
		printf("s -- Silent mode. Default by 0\n");
		printf("c -- Cache capacity\n");
		printf("a -- Cache associativity\n");
		printf("l -- Cache line byte size\n");
		printf("f -- trace.din to be run for the simulator\n");
		printf("Example: ./bin/llc -f <filename> -c 16000 -a 8\n");
		return 0;
            case '?':
                if(isprint(optopt))
                    printf ("Unknown option `-%c'.\n", optopt);
                else
                    printf ("Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                abort();
        }
    }
    
    // Set global vars based on arguments
    BYTESELECTMASK = makeMask(BYTESELECTWIDTH);
    INDEXWIDTH = CAPACITY - log2(ASSOCIATIVITY) - BYTESELECTWIDTH;
    INDEXMASK = makeMask(INDEXWIDTH);
    TAGWIDTH = pow(2,ADDRWIDTH-3) - INDEXWIDTH - BYTESELECTWIDTH;
    TAGMASK = makeMask(TAGWIDTH);
    SETS = pow(2,INDEXWIDTH);
#ifdef DEBUG
    printf("Debug mode is %s\n", (normalMode) ? "normal" : "silent");
    printf("s=%d A=%d c=%d a=%d l=%d f=%s\n", normalMode, ADDRWIDTH, CAPACITY, ASSOCIATIVITY, BYTESELECTWIDTH, fileName);
    printf("BMask=%x I=%d IMask=%x T=%d TMask=%x\n", BYTESELECTMASK, INDEXWIDTH, INDEXMASK, TAGWIDTH, TAGMASK);
#endif

    // Variables needed for file operations
    FILE *fptr = NULL;
    char buffer[FILEBUFFS];

    fptr = fopen(fileName, "r");
    if (fptr == NULL) {
        printf("Error opening file %s\n", fileName);
        return 1;
    }
    LLC = cacheInit();
    while(fgets(buffer, FILEBUFFS, fptr) != NULL) {
    // Do processing for event
#ifdef DEBUG
        printf("file read: %s", buffer);
#endif  
    Trace event = ParseTrace(&buffer[0]);
    cache(event);
    // Process the buffer
    }
    // clean up allocated objects before exiting
    displayTraceResult();
    cacheDestroy(LLC);
    fclose(fptr);
    return 0;
}

uint32_t makeMask(uint32_t maskWidth){
    uint32_t mask = 0;
    
    for(int i = 0; i < maskWidth; i++)
        mask = ((mask << 1) | 1);
        
    return mask;
}
