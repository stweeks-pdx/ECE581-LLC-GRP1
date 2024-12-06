#include "cache.h"
#include "trace.h"
#include <stdbool.h>

static cacheStruct LLC;

static uint32_t hits;
static uint32_t misses;
static uint32_t reads;
static uint32_t writes;

void cache(Trace request){ //Switch case for n here?
	switch(request.n){
		case 0:
		case 2: reads++;
			if(checkForPresence(request.tag, request.index) == HIT)	
				break;
			else{
				store(request.tag, request.index, request.n, request.address);
				break;
			}
		case 1: writes++;
			if(checkForPresence(request.tag, request.index) == HIT)
				break;
			else{
				store(request.tag, request.index, request.n, request.address);
				break;
			}
		case 3:


		
return;
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

void store(uint16_t tag, uint16_t index, uint8_t command, uint32_t address){
	int victim = 0;
	int emptyWay = setNotFull(LLC.cache[index]);
	if(emptyWay == -1){
		victim = victimPLRU(LLC.cache[index].plru);
		if(command != 1){ 
	  		if(getState(index,victim) == INVALID)
        			busOperation(READ,address);
    		}
    		else {
      			if(getState(index,victim) == INVALID)
			        busOperation(WRITE,address);
  			else if(getState(index,victim) == SHARED)
			        busOperation(UPGRADE,address);
    		}
  		LLC.cache[index].myWay[victim].tag = tag;
  		updateState(index,victim,command,getSnoopResult(address));
 			updatePLRU(LLC.cache[index].plru,victim);
			messageToL1(EVICT,address);
	}
  else{
		if(command !=1){
		  if(getState(index,emptyWay) == INVALID)
	 			busOperation(READ,address);
		}
		else{
			if(getState(index,emptyWay) == INVALID)
				busOperation(WRITE,address);
			else if(getState(index,emptyWay) == SHARED)
				busOperation(UPGRADE,address);
	  }
		  LLC.cache[index].myWay[emptyWay].tag = tag;
		  updateState(index,emptyWay,command,getSnoopResult(address));	
		  updatePLRU(LLC.cache[index].plru,emptyWay);
  }
}


void busOperation(busCmds command,uint32_t address){
	snoopResult = getSnoopResult(address);
	if(normalMode)
		printf("BusOp: %d, Address: %h, Snoop Result %d\n",command,address,snoopResult);
	}

snoopResult getSnoopResult(uin32_t address){
    	return (address & 0x3); 
}

/*putSnoopResult:Simulate our snooping of other caches*/
void putSnoopResult(uint32_t address, snoopResult message){
	if(normalMode)
		printf("snoopResult: %d, Address: %h\n",message,address); 
}	

/*Simulate communication to L1 cache*/
void messageToL1(inclusiveMsg message, uint32_t address){
	if(normalMode)
		printf("L2: %d %h\n",message,address);
}

void updatePLRU(bool plru[],int way){
	uint8_t index = 0;
	uint8_t level = 

}

int victimPLRU(bool plru[]){

}

mesi getState(uint16_t index, int way){
	return LLC.cache[index].myWay[way].state;
}

void updateState(uint16_t index, int way, uint8_t command, snoopResult result){
	switch(getState(index,way)){
		case INVALID:if((command == 0 || command == 2) && (result == HIT || result == HITM))
				     LLC.cache[index].myWay[way].state = SHARED;
			     else if(command == 0 || command == 2 && result == MISS)
				     LLC.cache[index].myWay[way].state = EXCLUSIVE;
			     else if(command == 1)
				     LLC.cache[index].myWay[way].state = MODIFIED;
			     else
				     LLC.cache[index].myWay[way].state = INVALID;
			     break;
		case SHARED:if(command == 0 || command == 2 || command == 3)
				     LLC.cache[index].myWay[way].state = SHARED;
			     else if(command == 1)
				     LLC.cache[index].myWay[way].state = MODIFIED;
			     else
				     LLC.cache[index].myWay[way].state = INVALID; 
			     break;
		case MODIFIED:if(command == 3)
				     LLC.cache[index].myWay[way].state = SHARED;
			     else if(command == 5)
				     LLC.cache[index].myWay[way].state = INVALID;
			     else
				     LLC.cache[index].myWay[way].state = MODIFIED;
			     break;
		case EXCLUSIVE:if(command == 1)
				     LLC.cache[index].myWay[way].state = MODIFIED;
			     else if(command == 3)
				     LLC.cache[index].myWay[way].state = SHARED;
			     else if(command == 5)
				     LLC.cache[index].myWay[way].state = INVALID;
			     else
				     LLC.cache[index].myWay[way].state = EXCLUSIVE;
			     break;
	}
}

			        


	

//write coherence broadcast function

//dataInL1 function

//tellL1ToFlush function

//MESI state transition
