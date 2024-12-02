#include "cache.h"
#include "trace.h"

static cacheStruct LLC;

static uint32_t hits;
static uint32_t misses;
static uint32_t reads;
static uint32_t writes;

void cache(Trace request){
if (L1req)
	//Do L1 stuff
else
	//Do coherence stuff
return;

}

requestResult checkForPresence(uint16_t tag, uint16_t index){
	for(int i=0; i<ASSOCIATIVITY; i++){
		if (LLC.cache[index].myWay[i].tag == tag){
			if (LLC.cache[index].myWay[i].state != INVALID){
			updatePLRU(LLC.cache[index].plru, i);
			hits++;
			//PLACEHOLDER FOR OUTPUT MESSAGING???
			return HIT;
			}
		}
	}
	misses++;
	//PLACEHOLDER FOR OUTPUT MESSAGING???
	return MISS;
}


int setNotFull(uint16_t index){
	for(int i=0; i<ASSOCIATIVITY; i++){
		if(getState(index, i) == INVALID)
			return i;
	}
return -1;
}

void messageToL1(inslusiveMsg message, uint32_t address){
	if (normal)
		printf("L2: %d %h\n", message, address);
}

void displayTraceResult(uint32_t hits, uint32_t misses, uint32_t reads, uint32_t writes){
	printf("Hits: %d, Misses: %d, Reads: %d, Writes: %d, Hit ratio: %f", hits, misses, reads, writes, hits/misses);
}

void printCache(void){
	for (int i=0; i<SETS; i++)
		for (int j=0; j<ASSOCIATIVITY; j++)
			if(getState(i, j) !=INVALID)
				printf("Tag = %X, State = %d", LLC.cache[i].myWay[j].tag, LLC.cache[i].myWay.state); 
}

void resetCache(void){
	for (int i=0; i<SETS; i++)
		for (int j=0; j<ASSOCIATIVITY-1, j++)
			LLC.cache[i].plru[j] = false;
		for (int j=0; j<ASSOCIATIVITY; j++)
			LLC.cache[i].myWay[j].state = INVALID;
}
