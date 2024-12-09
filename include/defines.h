#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <stdint.h>

#define ADDRWIDTH 8
#define FILELOC 1
#define FILEBUFFS 60

//Defines for Parsing shifts and masks
#define ADDRESSWIDTH    32

extern uint32_t BYTESELECTMASK;
extern uint32_t INDEXWIDTH;
extern uint32_t INDEXMASK;
extern uint32_t TAGWIDTH;
extern uint32_t TAGMASK;

#endif // _DEFINES_H_
