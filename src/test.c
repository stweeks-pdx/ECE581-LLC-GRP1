#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <stdlib.h>

// Global variables from our arguments
int ADDRWIDTH = 32;
int CAPACITY = 24;
int ASSOCIATIVITY = 16;
int BYTESELECTWIDTH = 6;
char *fileName = "..//testing//rwinms.din";

// Global variables set by arguments
uint8_t normalMode = 1;
int BYTESELECTMASK = 0;// 0x3F;
int INDEXWIDTH = 0;//14;
int INDEXMASK = 0;//0x3FFF;
int TAGWIDTH = 0;//12;
int TAGMASK = 0;//0xFFF;

// makeMask(maskWidth) creates & mask based on width
int makeMask(int maskWidth);

int main(int argc, char *argv[]) {
	int c;
	
	// Parse arguments
	while((c = getopt(argc, argv, "sA:c:a:l:f:")) != -1){
		switch(c){
			case 's':
				normalMode = 0;
				break;
			case 'A': 
				ADDRWIDTH = atoi(optarg);
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
	TAGWIDTH = ADDRWIDTH - INDEXWIDTH - BYTESELECTWIDTH;
	TAGMASK = makeMask(TAGWIDTH);
	
	printf("s=%d A=%d c=%d a=%d l=%d f=%s\n", normalMode, ADDRWIDTH, CAPACITY, ASSOCIATIVITY, BYTESELECTWIDTH, fileName);
	printf("BMask=%x I=%d IMask=%x T=%d TMask=%x\n", BYTESELECTMASK, INDEXWIDTH, INDEXMASK, TAGWIDTH, TAGMASK);
	return 0;
}

int makeMask(int maskWidth){
	int mask = 0;
	
	for(int i = 0; i < maskWidth; i++)
		mask = ((mask << 1) | 1);
		
	return mask;
}
