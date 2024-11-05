#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "trace.h"

Trace ParseTrace(char* buffer) {
	char tempAddress[ADDRWIDTH];

	Trace event = {};

        event.n = buffer[0] - '0';
	strncpy(tempAddress, &buffer[2], ADDRWIDTH);
	event.address = strtoul(tempAddress, NULL, 16);	
	
#ifdef DEBUG
	printf("CMD = %d\taddress = %08x\n", event.n, event.address);
#endif	

	return event;
}

