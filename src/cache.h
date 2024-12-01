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
	UPGRADE;
	FLUSH;
} busCmds

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


