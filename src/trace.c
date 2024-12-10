#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "defines.h"
#include "trace.h"

Trace ParseTrace(char* buffer) {
    char tempAddress[ADDRWIDTH];
#ifdef DEBUG
    char comment[FILEBUFFS];
#endif
    Trace event = {};

    // Parse command and address or comment
        event.n = buffer[0] - '0';
    if(event.n == 7) { //Internal NOP command for comments
#ifdef DEBUG
        strncpy(comment, &buffer[2], FILEBUFFS);
        printf("%s", comment);
#endif
        return event;
    } // else read address
    strncpy(tempAddress, &buffer[2], ADDRWIDTH);
    event.address = strtoul(tempAddress, NULL, 16); 
    
    // Parse address into tag, index, and byte
    event.byte = event.address & BYTESELECTMASK;
    event.index = (event.address >> BYTESELECTWIDTH) & INDEXMASK;
    event.tag = (event.address >> (BYTESELECTWIDTH + INDEXWIDTH)) & TAGMASK;
    
#ifdef DEBUG
    printf("CMD = %d\taddress = %08x\t tag = %03x index = %04x byte = %02x\n", event.n, event.address, event.tag, event.index, event.byte);
#endif  

    return event;
}
