#include "cache.h"
#include <stdio.h>
#include <math.h>

static cacheStruct LLC;

static uint32_t hits;
static uint32_t misses;
static uint32_t reads;
static uint32_t writes;

static char busOpLUT[4][11] = {"READ", "WRITE", "INVALIDATE", "RWIM"};
static char snoopLUT[4][6] = {"MISS", "HIT", "HITM", "NOHIT"};
static char l1LUT[4][15] = {"GETLINE", "SENDLINE", "INVALIDATLINE", "EVICTLINE"};


void cache(Trace request){ 
  int way = findWay(request.index, request.tag);
	switch(request.n){
		case L1DATAREAD:
		case L1INSTREAD: reads++;
			if(checkForPresence(request.tag, request.index) == HIT)	
				break;
			else{
				store(request.tag, request.index, request.n, request.address);
				break;
			}
		case L1WRITE: writes++;
			if(checkForPresence(request.tag, request.index) == HIT)
				break;
			else{
				store(request.tag, request.index, request.n, request.address);
				break;
			}
		case SNOOPEDREAD:
      if(checkForPresence(request.tag, request.index) == HIT){
        if(getState(request.index, request.tag) == MODIFIED){
          putSnoopResult(request.address, HITM);
        }
        else {
          putSnoopResult(request.address, HIT);
        }
      }
      else {
        putSnoopResult(request.address, NOHIT);
      }
      updateState(request.index, way, request.n, getSnoopResult(request.address), request.tag,request.address);
      break;
    case SNOOPEDWRITE:
      break;
    case SNOOPEDRWIM:
      if(checkForPresence(request.tag, request.index) == HIT){
        if(getState(request.index, request.tag) == MODIFIED){
          putSnoopResult(request.address, HITM);
        }
        else {
          putSnoopResult(request.address, HIT);
        }
      }
      else {
        putSnoopResult(request.address, NOHIT);
      }
      updateState(request.index, way, request.n, getSnoopResult(request.address), request.tag, request.address);
      break;
    case SNOOPEDINVAL: 
      if(checkForPresence(request.tag, request.index == HIT)){
        if(getState(request.index, request.tag) == MODIFIED){
          putSnoopResult(request.address, HITM);
        }
        else{
          putSnoopResult(request.address, HIT);
        }
      }
      else{
        putSnoopResult(request.address, NOHIT);
      }
      updateState(request.index, way, request.n, getSnoopResult(request.address), request.tag, request.address);
      break;
    case CLEARCACHE:
      resetCache();
      break;
    case PRINTCACHE:
      printCache();
      break;
  }
return;
}

int findWay(uint16_t index, uint16_t tag){
  for(int i = 0; i < ASSOCIATIVITY; i++){
    if(LLC.cache[index].myWay[i].tag == tag){
      return i;
    }
  }
  return -1;
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
	uint32_t victimAddress = 0;
	if(emptyWay == -1){
		victim = victimPLRU(LLC.cache[index].plru);
		victimAddress |= (LLC.cache[index].myWay[victim].tag << (INDEXWIDTH + BYTESELECTWIDTH)) | 
					(index << BYTESELECTWIDTH);
#ifdef DEBUG
		printf("Selected victim: %d \n", victim);
#endif
  		LLC.cache[index].myWay[victim].tag = tag;
		LLC.cache[index].myWay[victim].state = INVALID;

 			updatePLRU(LLC.cache[index].plru, victim);
			if(getState(index, tag) == MODIFIED) {
				victimAddress |= 0x3;
				messageToL1(EVICTLINE, victimAddress);
				busOperation(WRITE, victimAddress);
			}
			else
				messageToL1(INVALIDATELINE, victimAddress);
  		updateState(index, victim, command, getSnoopResult(address), tag, address);
	}
  else{
		  LLC.cache[index].myWay[emptyWay].tag = tag;
		  updateState(index, emptyWay, command, getSnoopResult(address), tag, address);	
		  updatePLRU(LLC.cache[index].plru, emptyWay);
  }
}


void busOperation(int command,uint32_t address){
	int result = getSnoopResult(address);
	if(normalMode)
		printf("BusOp: %s, Address: %X, Snoop Result %s \n", busOpLUT[command], address, snoopLUT[result]);
	}

int getSnoopResult(uint32_t address){
	int returnMe = NOHIT;
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
		printf("int: %s, Address: %X \n",snoopLUT[message], address); 
}	

char getState(uint16_t index, uint16_t tag){
  for(int i = 0; i < ASSOCIATIVITY; i++){
    if(LLC.cache[index].myWay[i].tag == tag){
	    return LLC.cache[index].myWay[i].state;
    }
  }
  return INVALID; 
}

// TODO: change getnSnoopResult call inside the function to optimize
void updateState(uint16_t index, int way, uint8_t command, int result, uint16_t tag, uint32_t address){
	switch(getState(index,tag)){
		case INVALID:if((command == L1DATAREAD || command == L1INSTREAD) && (result == HIT || result == HITM)){
				     busOperation(READ,address);
				     LLC.cache[index].myWay[way].state = SHARED;
			     }
			     else if((command == L1DATAREAD || command == L1INSTREAD) && (result == NOHIT)){
				     busOperation(READ,address);
				     LLC.cache[index].myWay[way].state = EXCLUSIVE;
			     }
			     else if(command == L1WRITE){
				     busOperation(RWIM,address);
				     LLC.cache[index].myWay[way].state = MODIFIED;
			     }
			     else
				     LLC.cache[index].myWay[way].state = INVALID;
			     break;
		case SHARED:if(command == L1DATAREAD || command == L1INSTREAD || command == SNOOPEDREAD)
				     LLC.cache[index].myWay[way].state = SHARED;
			     else if(command == L1WRITE){
				     LLC.cache[index].myWay[way].state = MODIFIED;
				     busOperation(INVALIDATE,address);
			     }
			     else if (command == SNOOPEDRWIM || SNOOPEDINVAL)
				     LLC.cache[index].myWay[way].state = INVALID; 
			     break;
		case MODIFIED:if(command == SNOOPEDREAD){
				     busOperation(WRITE,address);
				     LLC.cache[index].myWay[way].state = SHARED;
			     }
			     else if(command == SNOOPEDRWIM){
				     busOperation(WRITE,address);
				     LLC.cache[index].myWay[way].state = INVALID;
			     }
			     else
				     LLC.cache[index].myWay[way].state = MODIFIED;
			     break;
		case EXCLUSIVE:if(command == L1WRITE)
				     LLC.cache[index].myWay[way].state = MODIFIED;
			     else if(command == SNOOPEDREAD)
				     LLC.cache[index].myWay[way].state = SHARED;
			     else if(command == SNOOPEDRWIM)
				     LLC.cache[index].myWay[way].state = INVALID;
			     else
				     LLC.cache[index].myWay[way].state = EXCLUSIVE;
			     break;
	}
}

int setNotFull(uint16_t index){
	for(int i = 0; i < ASSOCIATIVITY; i++){
		if(LLC.cache[index].myWay[i].state == INVALID)	
			return i;
	}
	return -1;
}

void messageToL1(int message, uint32_t address){
	if (normalMode)
		printf("L2: %s %X \n", l1LUT[message], address);
}

void displayTraceResult(){
	float hitRate = (float) hits/misses;
	printf("Hits: %d, Misses: %d, Reads: %d, Writes: %d, Hit ratio: %f \n", hits, misses, reads, writes, hitRate);
}

void printCache(void){
	bool printPLRU = false;
	for (int i=0; i<SETS; i++) {
		for (int j=0; j<ASSOCIATIVITY; j++) {
			if(getState(i, LLC.cache[i].myWay[j].tag) != INVALID){ 
				printf("Tag = %X, Set = %X, State = %c, Way = %d \n", LLC.cache[i].myWay[j].tag, i, LLC.cache[i].myWay->state, j); 
				printPLRU = true;
			}
		}
		if (printPLRU){
			printf("PLRU for set %d: ", i);
				for (int j=0; j<ASSOCIATIVITY-1; j++){
					printf("%d ", LLC.cache[i].plru[j]);
				}
			printf("\n\n");
			printPLRU = false;
		}
	}
}

void resetCache(void){
	for (int i=0; i<SETS; i++){
		for (int j=0; j<ASSOCIATIVITY-1; j++) {
			LLC.cache[i].plru[j] = 0;
		}
		for (int j=0; j<ASSOCIATIVITY; j++) {
			LLC.cache[i].myWay[j].state = INVALID;
		}
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
		way |= (~plru[index] & 0x1) << (level-1);
		index = ((index + 1)*2+(~plru[index] & 0x01))-1; 
	}
	return way;
}

void cacheInit(void){
  for (int i = 0; i < SETS; i++){
    for (int j = 0; j < ASSOCIATIVITY; j++){
      LLC.cache[i].myWay[j].state = INVALID;
    }
  }
  plruInit();
}

void plruInit(void){
	for (int i = 0; i < SETS; i++){
		for (int j = 0; j < ASSOCIATIVITY-1; j++){
			LLC.cache[i].plru[j] = 0;
		}
	}
}
