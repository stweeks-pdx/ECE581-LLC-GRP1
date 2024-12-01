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

void serviceL1Cache(Trace L1Request){

	if(checkForPresence(L1Request.tag, L1Request.index) == HIT){
		switch (L1Request.n){
		0:
		2: reads++;
		   break;
		1: writes++;
		   break;
		}
	}
	else {
	store(L1Request.index, L1Request.tag);
}

void serviceCoherence(Trace busRequest){
}

requestResult checkForPresence(uint16_t tag, uint16_t index){
	for(int i=0; i<ASSOCIATIVITY; i++){
		if (LLC.cache[index].myWay[i].tag == tag){
		updatePLRU(LLC.cache[index].plru, i);
		hits++;
		return HIT;
		}
	}
	misses++;
	return MISS;
}

void store(uint16_t index, uint16_t tag, uint8_t command){
	int victim = 0;
	if(setIsFull(LLC.cache[index])){
		victim = victimPLRU(LLC.cache[index].plru);
		if((LLC.cache[index].myWay[victim].state) == MODIFIED)
			busOp(WRITE, 
		LLC.cache[index].myWay[victim].tag = tag;
		mesiStateUpdate(index, tag, command);
		if (dataInL1)
			tellL1ToFlush();
	}
}

//write coherence broadcast function

//dataInL1 function

//tellL1ToFlush function

//MESI state transition
