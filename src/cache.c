#include "cache.h"
#include <stdio.h>
#include <math.h>

static uint32_t hits;
static uint32_t misses;
static uint32_t reads;
static uint32_t writes;

static char busOpLUT[4][11] = {"READ", "WRITE", "INVALIDATE", "RWIM"};
static char snoopLUT[4][6] = {"MISS", "HIT", "HITM", "NOHIT"};
static char l1LUT[4][15] = {"GETLINE", "SENDLINE", "INVALIDATELINE", "EVICTLINE"};

cacheStruct *LLC = NULL;

void cache(Trace request){ 
    int way = findWay(request.index, request.tag);
    switch(request.n){
        case L1DATAREAD:
        case L1INSTREAD: reads++;
            if(checkForPresence(request.tag, request.index) == HIT){
                hits++; 
                messageToL1(SENDLINE, request.address);
            }   
            else{
                misses++;
                store(request.tag, request.index, request.n, request.address);
            }
            break;
        case L1WRITE: writes++;
            if(checkForPresence(request.tag, request.index) == HIT) {
                hits++;
                updateState(request.index, way, request.n, 
                        getSnoopResult(request.address), request.tag, request.address);
            }
            else{
                misses++;
                store(request.tag, request.index, request.n, request.address);
            }
            messageToL1(GETLINE, request.address);
            break;
        case SNOOPEDREAD:
            if(checkForPresence(request.tag, request.index) == HIT){
                if(getState(request.index, request.tag) == MODIFIED){
                    messageToL1(GETLINE, request.address);
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
                    messageToL1(EVICTLINE, request.address);
                }
                else {
                    putSnoopResult(request.address, HIT);
                    messageToL1(INVALIDATELINE, request.address);
                }
            }
            else {
                putSnoopResult(request.address, NOHIT);
            }
            updateState(request.index, way, request.n, getSnoopResult(request.address), request.tag, request.address);
            break;
        case SNOOPEDINVAL: 
            if(checkForPresence(request.tag, request.index) == HIT){
                if(getState(request.index, request.tag) == SHARED){
                    messageToL1(INVALIDATELINE, request.address);
                }
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
        if(LLC->cache[index].myWay[i].tag == tag){
        return i;
    }
    }
    return -1;
}


int checkForPresence(uint16_t tag, uint16_t index){
    for(int i=0; i < ASSOCIATIVITY; i++){
        if (LLC->cache[index].myWay[i].tag == tag){
            if (LLC->cache[index].myWay[i].state != INVALID){
                updatePLRU(LLC->cache[index].plru, i);
            return HIT;
            }
        }
    }
    return MISS;
}

void store(uint16_t tag, uint16_t index, uint8_t command, uint32_t address){
    int victim = 0;
    int emptyWay = setNotFull(index);
    uint32_t victimAddress = 0;
    if(emptyWay == -1){
        victim = victimPLRU(LLC->cache[index].plru);
        victimAddress |= (LLC->cache[index].myWay[victim].tag << (INDEXWIDTH + BYTESELECTWIDTH)) | 
                    (index << BYTESELECTWIDTH);
#ifdef DEBUG
        printf("Selected victim: %02d \n", victim);
#endif
        LLC->cache[index].myWay[victim].tag = tag;

        updatePLRU(LLC->cache[index].plru, victim);
        if(getState(index, tag) == MODIFIED) {
            victimAddress |= 0x3;
            messageToL1(EVICTLINE, victimAddress);
            busOperation(WRITE, victimAddress);
        }
        else
            messageToL1(INVALIDATELINE, victimAddress);
        
        LLC->cache[index].myWay[victim].state = INVALID;
        updateState(index, victim, command, getSnoopResult(address), tag, address);
    }
    else{
          LLC->cache[index].myWay[emptyWay].tag = tag;
          updateState(index, emptyWay, command, getSnoopResult(address), tag, address); 
          updatePLRU(LLC->cache[index].plru, emptyWay);
    }
    messageToL1(SENDLINE, address);
}


void busOperation(int command,uint32_t address){
    int result = getSnoopResult(address);
    if(normalMode)
        printf("BusOp: %s, Address: %08X, Snoop Result %s \n", busOpLUT[command], (address&(((TAGMASK << INDEXWIDTH)|INDEXMASK) << BYTESELECTWIDTH)) , snoopLUT[result]);
}

int getSnoopResult(uint32_t address){
    int returnMe = NOHIT;
    switch (address & MASK2LSB){
        case 0x2: 
        case 0x3: returnMe = NOHIT;
            break;
        case 0x0: returnMe = HIT;
            break;
        case 0x1: returnMe = HITM;
            break;
    }
    return returnMe;
}

/*putSnoopResult:Simulate our snooping of other caches*/
void putSnoopResult(uint32_t address, int message){
    if(normalMode)
        printf("int: %s, Address: %08X \n",snoopLUT[message], address); 
}   

char getState(uint16_t index, uint16_t tag){
    for(int i = 0; i < ASSOCIATIVITY; i++){
        if(LLC->cache[index].myWay[i].tag == tag){
            return LLC->cache[index].myWay[i].state;
        }
    }
    return INVALID; 
}

void updateState(uint16_t index, int way, uint8_t command, int result, uint16_t tag, uint32_t address){
    switch(getState(index,tag)){
        case INVALID:
            if((command == L1DATAREAD || command == L1INSTREAD) && (result == HIT || result == HITM)){
                busOperation(READ,address);
                 LLC->cache[index].myWay[way].state = SHARED;
            }
            else if((command == L1DATAREAD || command == L1INSTREAD) && (result == NOHIT)){
                 busOperation(READ,address);
                 LLC->cache[index].myWay[way].state = EXCLUSIVE;
            }
            else if(command == L1WRITE){
                 busOperation(RWIM,address);
                 LLC->cache[index].myWay[way].state = MODIFIED;
            }
            else
                 LLC->cache[index].myWay[way].state = INVALID;
            break;
        case SHARED:
            if(command == L1DATAREAD || command == L1INSTREAD || command == SNOOPEDREAD)
                LLC->cache[index].myWay[way].state = SHARED;
            else if(command == L1WRITE){
                LLC->cache[index].myWay[way].state = MODIFIED;
                busOperation(INVALIDATE,address);
            }
            else if (command == SNOOPEDRWIM || SNOOPEDINVAL)
                 LLC->cache[index].myWay[way].state = INVALID; 
            break;
        case MODIFIED:
            if(command == SNOOPEDREAD){
                busOperation(WRITE,address);
                LLC->cache[index].myWay[way].state = SHARED;
            }
            else if(command == SNOOPEDRWIM){
                busOperation(WRITE,address);
                LLC->cache[index].myWay[way].state = INVALID;
            }
            else
                LLC->cache[index].myWay[way].state = MODIFIED;
            break;
        case EXCLUSIVE:
            if(command == L1WRITE)
                LLC->cache[index].myWay[way].state = MODIFIED;
            else if(command == SNOOPEDREAD)
                LLC->cache[index].myWay[way].state = SHARED;
            else if(command == SNOOPEDRWIM)
                LLC->cache[index].myWay[way].state = INVALID;
            else
                LLC->cache[index].myWay[way].state = EXCLUSIVE;
            break;
    }
}

int setNotFull(uint16_t index){
    for(int i = 0; i < ASSOCIATIVITY; i++){
        if(LLC->cache[index].myWay[i].state == INVALID) 
            return i;
    }
    return -1;
}

void messageToL1(int message, uint32_t address){
    if (normalMode)
        printf("L2: %s %08X \n", l1LUT[message], address);
}

void displayTraceResult(){
    float hitRate = (float) hits/(writes+reads);
    printf("Hits: %d, Misses: %d, Reads: %d, Writes: %d, Hit ratio: %f \n", hits, misses, reads, writes, hitRate);
}

void printCache(void){
    bool printPLRU = false;
    printf("\nPRINTING CACHE\n++++++++++++++++\n");
    for (int i=0; i<SETS; i++) {
        for (int j=0; j<ASSOCIATIVITY; j++) {
            if(LLC->cache[i].myWay[j].state != INVALID){ 
                printf("Tag = %03X, Set = %04X, State = %c, Way = %02d \n", LLC->cache[i].myWay[j].tag, i, LLC->cache[i].myWay[j].state, j); 
                printPLRU = true;
            }
        }
        if (printPLRU){
            printf("PLRU for set %d: ", i);
                for (int j=0; j<ASSOCIATIVITY-1; j++){
                    printf("%d ", LLC->cache[i].plru[j]);
                }
            printf("\n\n");
            printPLRU = false;
        }
    }
    printf("================\n\n");
}

void resetCache(void){
    for (int i=0; i<SETS; i++){
        for (int j=0; j<ASSOCIATIVITY-1; j++) {
            LLC->cache[i].plru[j] = 0;
        }
        for (int j=0; j<ASSOCIATIVITY; j++) {
            LLC->cache[i].myWay[j].state = INVALID;
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

cacheStruct *cacheInit(void){
#ifdef DEBUG
    printf("INITIALIZING CACHE\n");
#endif
    LLC = (cacheStruct*)malloc(sizeof(cacheStruct));
    // Allocate space for each element within cache
    LLC->cache = (set*)malloc(SETS * sizeof(set));
    for (int i = 0; i < SETS; i++){
        LLC->cache[i].myWay = (way*)malloc(ASSOCIATIVITY * sizeof(way));
        for(int j = 0; j < ASSOCIATIVITY; j++){
            LLC->cache[i].myWay[j].tag = 0;
            LLC->cache[i].myWay[j].state = INVALID;
        }
        LLC->cache[i].plru = (uint8_t*)malloc((ASSOCIATIVITY-1) * sizeof(uint8_t)); 
    }

    for (int i = 0; i < SETS; i++){
        for (int j = 0; j < ASSOCIATIVITY; j++){
            LLC->cache[i].myWay[j].state = INVALID;
        }
    }
    plruInit();
    return LLC;
}

void cacheDestroy(cacheStruct *LLC){
	for(int i = 0; i < SETS; i++){
		free(LLC->cache[i].myWay);
		free(LLC->cache[i].plru);
		LLC->cache[i].myWay = NULL;
		LLC->cache[i].plru = NULL;
	}
	free(LLC->cache);
	LLC->cache = NULL;
	free(LLC);
	LLC = NULL;
}

void plruInit(void){
    for (int i = 0; i < SETS; i++){
        for (int j = 0; j < ASSOCIATIVITY-1; j++){
            LLC->cache[i].plru[j] = 0;
        }
    }
}
