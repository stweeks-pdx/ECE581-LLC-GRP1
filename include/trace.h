#ifndef _TRACE_H_
#define _TRACE_H_

#include <stdint.h>

#include "defines.h"

typedef struct Traces{
	uint8_t n;
	uint32_t address;
} Trace;

Trace ParseTrace(char* buffer);

#endif // _TRACE_H_
