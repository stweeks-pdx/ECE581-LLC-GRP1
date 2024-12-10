#ifndef _TRACE_H_
#define _TRACE_H_

#include <stdint.h>
#include <stdlib.h>
#include "defines.h"

typedef struct Traces{
	uint8_t n;
	uint32_t address;
	uint8_t byte;
	uint16_t index;
	uint16_t tag;
} Trace;

Trace ParseTrace(char* buffer);

#endif // _TRACE_H_
