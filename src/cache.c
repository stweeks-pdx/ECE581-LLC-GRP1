#include "cache.h"
#include <stdio.h>
#include <math.h>

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
        if(getState(request.index,request.tag) == MODIFIED){
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
      updateState(request.index,request.tag,request.n,getSnoopResult(request.address));
      break;
    case 4:
      if(checkForPresence(request.tag,request.index) == HIT) {
        if(getState(request.index,request.tag) == MODIFIED){
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
      updateState(request.index,request.tag,request.n,getSnoopResult(request.address));
      break;
    case 5:
      if(checkForPresence(request.tag,request.index) == HIT){
        if(getState(request.index,request.tag) == MODIFIED){
          putSnoopResult(request.address,HITM);
        }
        else {
          putSnoopResult(request.address,HIT);
        }
      }
      else {
        putSnoopResult(request.address,NOHIT);
      }
      updateState(request.index,request.tag,request.n,getSnoopResult(request.address));
      break;
    case 6: 
      if(checkForPresence(request.tag,request.index == HIT)){
        if(getState(request.index,request.tag) == MODIFIED){
          putSnoopResult(request.address,HITM);
        }
        else{
          putSnoopResult(request.address,HIT);
        }
      }
      else{
        putSnoopResult(request.address,NOHIT);
      }
      updateState(request.index,request.tag,request.n,getSnoopResult(request.address));
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

int checkForPresence(uint16_t tag, uint16_t index){
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
			        busOperation(INVALIDATELINE,address);
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
				busOperation(INVALIDATELINE,address);
	  }
		  LLC.cache[index].myWay[emptyWay].tag = tag;
		  updateState(index,emptyWay,command,getSnoopResult(address));	
		  updatePLRU(LLC.cache[index].plru,emptyWay);
  }
}


void busOperation(int command,uint32_t address){
	int result = getSnoopResult(address);
	if(normalMode)
		printf("BusOp: %d, Address: %X, Snoop Result %d\n",command,address,result);
	}

int getSnoopResult(uint32_t address){
	int returnMe;
	switch (address & MASK2LSB){
		case 10:
		case 11: returnMe = NOHIT;
			break;
		case 00: returnMe = HIT;
			break;
		case 01: returnMe = HITM;
			break;
	}
	return returnMe;
}

/*putSnoopResult:Simulate our snooping of other caches*/
void putSnoopResult(uint32_t address, int message){
	
	if(normalMode)
		printf("int: %d, Address: %X \n",message,address); 
}	

char getState(uint16_t index, int way){
	return LLC.cache[index].myWay[way].state;
}

void updateState(uint16_t index, int way, uint8_t command, int result){
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

void messageToL1(int message, uint32_t address){
	if (normalMode)
		printf("L2: %d %X \n", message, address);
}

void displayTraceResult(uint32_t hits, uint32_t misses, uint32_t reads, uint32_t writes){
	float hitRate = hits/misses;
	printf("Hits: %d, Misses: %d, Reads: %d, Writes: %d, Hit ratio: %f", hits, misses, reads, writes, hitRate);
}

void printCache(void){
	for (int i=0; i<SETS; i++)
		for (int j=0; j<ASSOCIATIVITY; j++)
			if(getState(i, j) !=INVALID)
				printf("Tag = %X, State = %d", LLC.cache[i].myWay[j].tag, LLC.cache[i].myWay->state); 
}

void resetCache(void){
	for (int i=0; i<SETS; i++){
		for (int j=0; j<ASSOCIATIVITY-1; j++)
			LLC.cache[i].plru[j] = false;
		for (int j=0; j<ASSOCIATIVITY; j++)
			LLC.cache[i].myWay[j].state = INVALID;
	}
}



void updatePLRU(uint8_t plru[], int index){
	int level = log2(ASSOCIATIVITY);
	int curPlace = 0;
	for( ; level > 0 ; level--){
	plru[curPlace] = (index >> (level-1)) & 0x1;
	curPlace = ((curPlace + 1)*2+plru[curPlace])-1;
	}
}

int victimPLRU(uint8_t plru[]){
 	int way = 0;
 	int level = log2(ASSOCIATIVITY);
 	int index = 0;
 	
 	for( ; level > 0 ; level--){
	way |= ~plru[index] << (level-1);
	index = ((index + 1)*2+(~plru[index]))-1; 
	}
	return way;
}
