#include <stdint.h>
#ifndef _DEFINES_H_
#define _DEFINES_H_

#define FILELOC 1
#define FILEBUFFS 60
#define ADDRWIDTH 8

//Global variables
extern uint8_t normalMode;
extern uint32_t CAPACITY;
extern uint32_t ASSOCIATIVITY;
extern uint32_t BYTESELECTWIDTH;
extern uint32_t BYTESELECTMASK;
extern uint32_t INDEXWIDTH;
extern uint32_t INDEXMASK;
extern uint32_t TAGWIDTH;
extern uint32_t TAGMASK;
extern uint32_t SETS;

#endif // _DEFINES_H_
