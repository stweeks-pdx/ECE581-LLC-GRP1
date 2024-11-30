#ifndef _DEFINES_H_
#define _DEFINES_H_

#define ADDRWIDTH 8
#define FILELOC 1
#define FILEBUFFS 60

//Cache spec defines
#define ASSOCIATIVITY 16
#define SETS 16*1024

//Defines for Parsing shifts and masks
#define BYTESELECTWIDTH 6
#define BYTESELECTMASK	0x3F
#define INDEXWIDTH	14
#define INDEXMASK	0x3FFF
#define TAGWIDTH	12
#define TAGMASK		0xFFF

#endif // _DEFINES_H_
