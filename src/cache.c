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
      if(checkForPresence(request.tag,request.index) == HIT){
        if(getState(request.index,request.way) == MODIFIED){
          putSnoopResult(request.address,HITM);
          busOperation(WRITE,request.address);
        }
        else {
          putSnoopResult(request.address,HIT);
        }
      }
      else {
        putSnoopResult(request.address,NOHIT);
      }
      updateState(request.index,request.way,request.command,getSnoopResult(request.address));
      break;
    case 4:
      if(checkForPresence(request.tag,request.index) == HIT) {
        if(getState(request.index,request.way) == MODIFIED){
          putSnoopResult(request.address,HITM);
          busOperation(WRITE,request.address);
        }
        else {
          putSnoopResult(request.address,HIT);
        }
      }
      else {
        putSnoopResult(request.address,NOHIT);
      }
      updateState(request.index,request.way,request.command,getSnoopResult(request.address));
      break;
    case 5:
      if(checkForPresence(request.tag,request.index) == HIT){
        if(getState(request.index,request.way) == MODIFIED){
          putSnoopResult(request.address,HITM);
        }
        else {
          putSnoopResult(request.address,HIT);
        }
      }
      else {
        putSnoopResult(request.address,NOHIT);
      }
      updateState(request.index,request.way,request.command,getSnoopResult(request.address));
      break;
    case 6: 
      if(checkForPresence(request.tag,request.index == HIT)){
        if(getState(request.index,request.way) == MODIFIED){
          putSnoopResult(request.address,HITM);
        }
        else{
          putSnoopResult(request.address,HIT);
        }
      }
      else{
        putSnoopResult(request.address,NOHIT);
      }
      updateState(request.index,request.way,request.command,getSnoopResult(request.address));
      break;
    case 8:
      resetCache();
      break;
    case 9:
      printCache();
      break;
  }
return;
}

requestResult checkForPresence(uint16_t tag, uint16_t index){
	for(int i=0; i<ASSOCIATIVITY; i++){
		if (LLC.cache[index].myWay[i].tag == tag){
			if (LLC.cache[index].myWay[i].state != INVALID){
			updatePLRU(LLC.cache[index].plru, i);
			hits++;
			return HIT;
			}
		}
	}
	misses++;
	return MISS;
}

void store(uint16_t tag, uint16_t index, uint8_t command, uint32_t address){
	int victim = 0;
	int emptyWay = setNotFull(index);
	if(emptyWay == -1){
		victim = victimPLRU(LLC.cache[index].plru);
		if(command != 1){ 
	  		if(getState(index,victim) == INVALID)
        			busOperation(READ,address);
    		}
    		else {
      			if(getState(index,victim) == INVALID)
			        busOperation(RWIM,address);
  			else if(getState(index,victim) == SHARED)
			        busOperation(INVALIDATE,address);
    		}
  		LLC.cache[index].myWay[victim].tag = tag;
  		updateState(index,victim,command,getSnoopResult(address));
 			updatePLRU(LLC.cache[index].plru,victim);
			messageToL1(EVICTLINE,address);
	}
  else{
		if(command !=1){
		  if(getState(index,emptyWay) == INVALID)
	 			busOperation(READ,address);
		}
		else{
			if(getState(index,emptyWay) == INVALID)
				busOperation(RWIM,address);
			else if(getState(index,emptyWay) == SHARED)
				busOperation(INVALIDATE,address);
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
			     else if((command == 0 || command == 2) && (result == MISS))
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
