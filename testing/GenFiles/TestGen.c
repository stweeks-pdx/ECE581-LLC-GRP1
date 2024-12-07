#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "defines.h"

int main(int argc, char *argv[]) {
	if(argc < 3) {
		printf("TestGen <input file> <output file>\n");
		return 1;
	}
	
	char *inputName = NULL;
	FILE *inptr = NULL;
	char *outputName = NULL;
	FILE *outptr = NULL;

	char buff[FILEBUFFS], comment[FILEBUFFS];
	int command, hit;
	int setMin, setMax, tagMin, tagMax;
	uint32_t address;

	inputName = argv[1];
	outputName = argv[2];

	inptr = fopen(inputName, "r");
	if(inptr == NULL) {
		printf("Error opening file %s\n", inputName);
		return 1;
	}
	outptr = fopen(outputName, "w");
	if (outptr == NULL) {
		printf("Error opening file %s\n", outputName);	
		return 1;
	}

	while(fgets(buff, FILEBUFFS, inptr) != NULL){
		if(buff[0] == '7'){ // Comments are parsed seperately
			sscanf(buff, "%d %[^\n]", &command, comment);
			fprintf(outptr, "%d %s\n", command, comment);
			continue;
		} 		
		// else parse into relevent values
		sscanf(buff, "%d %d %d %d %d %d", &command, &setMin, &setMax, &tagMin, &tagMax, &hit); 
		address = 0;	
		for(int i = setMin; i <= setMax; i++){
			for(int j = tagMin; j <= tagMax; j++){
				//concatenate tag, index, bytesel, and write
				address = j;
				address = (address << INDEXWIDTH) | i;
				address = (address << BYTESELECTWIDTH) | hit;
				fprintf(outptr, "%d %08x\n", command, address);
			}
		}
	}				
	
	fclose(inptr);
	fclose(outptr);
}

