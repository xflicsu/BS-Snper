#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_funcs.h"
#include "chrome_funcs.h"
#include "sam_funcs.h"

int main(int argc, char **argv)
{
	int i, j;
	long cnt;
	FILE *bamFptr, *posFptr;
	
	//////////////////////////////////////////////////////////////////////////////
	// Parse command args
	//////////////////////////////////////////////////////////////////////////////
	// {{
	if(argc != 12) {
		printf("Not enough args! Argc = %d.\n", argc);
		exit(1);
	}
	
	char* chrLenFile = argv[1];
	char* refSeqFile = argv[2];
	char* bamFileName = argv[3];
	char* posFileName = argv[4];
	char* methFileName = argv[5]; // New add
	int vQualMin = atoi(argv[6]);
	int nLayerMin = atoi(argv[7]);
	int nLayerMax = atoi(argv[8]);
	float vSnpRate = atof(argv[9]);
	float vSnpPerBase = atof(argv[10]);
	unsigned int mapqThr = atoi(argv[11]);
	
	fprintf(stderr, "chrLenFile = %s.\n", chrLenFile);
	fprintf(stderr, "refSeqFile = %s.\n", refSeqFile);
	fprintf(stderr, "bamFileName = %s.\n", bamFileName);
	fprintf(stderr, "posFileName = %s.\n", posFileName);
	fprintf(stderr, "methFileName = %s.\n", methFileName);	// New add
	fprintf(stderr, "vQualMin = %d.\n", vQualMin);
	fprintf(stderr, "nLayerMin = %d.\n", nLayerMin);
	fprintf(stderr, "nLayerMax = %d.\n", nLayerMax);
	fprintf(stderr, "vSnpRate = %f.\n", vSnpRate);
	fprintf(stderr, "vSnpPerBase = %f.\n", vSnpPerBase);
	fprintf(stderr, "mapqThr = %d.\n", mapqThr);
	// }}
	
	//////////////////////////////////////////////////////////////////////////////
	// Load reference sequence & Init arrays
	//////////////////////////////////////////////////////////////////////////////
	// {{
	// Hash table
	int hash_table_size; 
	HashNode** hashTable = (HashNode**)malloc(sizeof(HashNode*) * HASH_TABLE_MAX_SIZE); 
	hash_table_init(hashTable, &hash_table_size);
	// Init chrome name-idx hash table
	int chrCnt;	
	init_chrome_hash(hashTable, &hash_table_size, chrLenFile, &chrCnt);
	fprintf(stderr, "Init chrome name-idx hash table completed.\n");
	// Init chrome name and length array
	int* chrLen = (int*)malloc(sizeof(int) * chrCnt);
	char** chrName = (char**)malloc(sizeof(char*) * chrCnt);
	for(i = 0; i < chrCnt; i++) 
		chrName[i] = (char*)malloc(sizeof(char) * 50);
	init_chrome_name_len(hashTable, chrLenFile, chrCnt, chrName, chrLen);
	cnt = 0; 
	for(i = 0; i < chrCnt; i++) 
		cnt += chrLen[i];
	fprintf(stderr, "Init chrome name-len array completed, total %ld bp of %d chromosomes.\n", cnt, chrCnt);
	// Init chrome seq array
	char** chrSeqArray = (char**)malloc(sizeof(char*) * chrCnt);
	for(i = 0; i < chrCnt; i++) 
		chrSeqArray[i] = (char*)malloc(sizeof(char) * chrLen[i] + 100);
	init_chrome_seq(hashTable, refSeqFile, chrSeqArray, chrLen);
    int* chrDone = (int*)malloc(sizeof(int) * chrCnt);
	for(i = 0; i < chrCnt; i++) {
		if(!(chrSeqArray[i] = (char*)realloc((void*)(chrSeqArray[i]), sizeof(char) * chrLen[i]))) {
			fprintf(stderr, "Not enough memory for chrSeqArray.\n");
			exit(1);
		}
		for(j = 0; j < chrLen[i]; j++)
			chrSeqArray[i][j] = toupper(chrSeqArray[i][j]);
        chrDone[i] = 0;
	}
	fprintf(stderr, "Init chrome seq array completed.\n");
	// }}
	
	//////////////////////////////////////////////////////////////////////////////
	// SNP process
	//////////////////////////////////////////////////////////////////////////////
	// {{
	snpAnalysis(bamFileName, posFileName, methFileName, hashTable, chrSeqArray, chrLen, chrDone, vQualMin, nLayerMin, nLayerMax, vSnpRate, vSnpPerBase, mapqThr);
	fprintf(stderr, "SNP process completed.\n");
	// }}

	return 0;
}
