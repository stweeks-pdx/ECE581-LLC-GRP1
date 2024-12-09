#ifndef _TRACE_H_
#define _TRACE_H_

#include <stdint.h>
#include <stdlib.h>
#include "defines.h"

//Global variables
extern uint8_t normalMode;
extern int ADDRWIDTH;
extern int CAPACITY;
extern int ASSOCIATIVITY;
extern int BYTESELECTWIDTH;
extern int BYTESELECTMASK;
extern int INDEXWIDTH;
extern int INDEXMASK;
extern int TAGWIDTH;
extern int TAGMASK;


typedef struct Traces{
	uint8_t n;
	uint32_t address;
	uint8_t byte;
	uint16_t index;
	uint16_t tag;
} Trace;

Trace ParseTrace(char* buffer);

#endif // _TRACE_H_
