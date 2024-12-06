#define MASK2LSB 0x3

typedef enum {
	INVALID;
	MODIFIED;
	EXCLUSIVE;
	SHARED;
} mesi;

typedef enum {
	MISS;
	HIT;
	HITM;
} snoopResult;

typedef enum {
	MISS;
	HIT;
} requestResult;

typedef enum {
	GET;
	SEND;
	INVALIDATE;
	EVICT;
} inclusiveMsg;

typedef enum { 
	READ;
	WRITE;
	INVALIDATE;
	RWIM;
} busCmd

typedef struct { 
	uint16_t tag;
	mesi state;
} way;

typedef struct { 
	way myWay[ASSOCIATIVITY];
	bool plru[ASSOCIATIVITY-1];
} set;

void update(way addressedWay);

uint32_t evict(void);

typedef struct {
	set cache[SETS];
} cacheStruct;

//TODO: to Nathan: enum members need to have unique names since they are global
//*******************FUNCTION PROTOTYPES**********************************

/*
Cache: Accepts a parsed trace and responds appropriately

Arguments:
	request: custom data type Trace that contains a command and address
*/
void cache(Trace request);

/*
checkForPresence: Uses index and tag arguments to determine if an address is present in the cache or not

Arguments:
	tag: unsigned 16 bit integer that associates data to unique physical address
	index: unsigned 16 bit integer that points us to the correct set for our tag

	returns requestResult, either hit or miss
*/
requestResult checkForPresence(uint16_t tag, uint16_t index);

/*
store: Uses index to find a place to store a particular tag. Uses command to broadcast proper snoop message on bus.

Arguments:
	tag: unsigned 16 bit interger that associated data to unique physical address
	index: unsigned 16 bit integer that points us to the correct set for our tag
	command: one of 9 commands for this cache simulation
*/
void store(uint16_t tag, uint16_t index, uint8_t command);

/* 
updatePLRU: Updates the plru bits based on recent memory access

Arguments:
	plru[]: The desired plru array that needs updated
	index: The index of the way that is being accessed, should be 0-15
*/
void updatePLRU(bool plru[], int index);

/*
setNotFull: Checks if a set has room

Arguments: 
	index: The set that we are checking

returns the index for the first empty way found, -1 if full.
*/
int setNotFull(uint16_t index);

/*
victimPLRU: Reads PLRU bits to find which way needs replaced on collision miss, then updates most recently accessed memory location

Arguments: 
	plru[]: The desired plru array used to point to eviction

Returns the index of the way that needs to be evicted
*/
int victimPLRU(bool plru[]);

/*
busOperation: Broadcasts the command and address IAW the MESI protocol based on what the cache just did

Arguemnts:
	busOp: The operation that we are putting out on the bus
	address: The address that other caches need to check for
*/
void busOperation(busCmd busOp, uint32_t address);

/*
getSnoopResult: Checks the trace to get the snoop result. For sim purposes, uses the 2LSB of the address.

Arguments:
	address: Address that we are acting on that needs snoop information for state changes

returns snoopResult based on 2LSB of address
*/
snoopResult getSnoopResult(uint32_t address);

/*
putSnoopResult: Tell other caches if we have an address or not

Arguments:
	address: The address that other caches are accessing
	message: Tells them if we have it, and if it is modified
*/
void putSnoopResult(uint32_t address, snoopResult message);

/*
messageToL1: Sends a message to L1 to maintain inclusivity

Arguments:
	message: Telling L1 what to do with an address
	address: What address L1 needs to act on
*/
void messageToL1(inclusiveMsg message, uint32_t address);

/*
getState: Gets the MESI state of a given way

Arguments:
	index: Tells us which set in the cache to access
	way: Which way in the set we need to access

Returns the MESI state of the requested cache line
*/
mesi getState(uint16_t index, int way);

/*
displayTraceResult: Displays the cache's usage statistic for a given simulation

Arguments:
	hits: The number of hits in this simulation
	misses: The number of misses in this simulation
	reads: The number of reads in this simulation
	writes: the number of writes in this simulation
*/
void displayTraceResult(uint32_t hits, uint32_t misses, uint32_t reads, uint32_t writes);

/*
resetCache: Invalidates all cache lines and resets all plrus
*/
void resetCache(void);

/*
printCache: Prints contents and state of each valid cache line without ending simulation
*/
void printCache(void);

/*
updateState: updates the MESI state of a given way

Arguments:
	index: Points us to the correct set in the cache
	way: The index for the specific cache line we are updating. Should be 0-15
	command: What command we are performing
	result: Snoop results from the bus
*/
void updateState(uint16_t index, int way, uint8_t command, snoopResult result);

