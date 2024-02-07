#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merge.h"

#define RECORDS_NUM 100 // you can change it if you want
#define FILE_NAME "data_modified.db"
#define OUT_NAME "out_modified"

int createAndPopulateHeapFile(char *filename);

void sortPhase(int file_desc, int chunkSize);

void mergePhases(int inputFileDesc, int chunkSize, int bWay, int *fileCounter);

int nextOutputFile(int *fileCounter);

void main()
{
  int chunkSize = 8; // Modified chunk size
  int bWay = 5;
  int fileIterator = 0;

  BF_Init(LRU);
  int file_desc = createAndPopulateHeapFile(FILE_NAME);
  
  printf("Records before sorting:\n\n");
  HP_PrintAllEntries(file_desc);

  sortPhase(file_desc, chunkSize);
  // HP_PrintAllEntries(file_desc);

  printf("-----------------------------------------------------------------\n");
  printf("Records after sorting:\n\n");
  
  mergePhases(file_desc, chunkSize, bWay, &fileIterator);
  
  char mergedFile[50];
  char tmp[] = "out_modified";
  for (int i = 0; i < fileIterator; i++)
  {
    sprintf(mergedFile, "%s%d.db", tmp, i);
    int output_file_desc;
    HP_OpenFile(mergedFile, &output_file_desc);
    HP_PrintAllEntries(output_file_desc);
    printf("\n");
  }
}

int createAndPopulateHeapFile(char *filename)
{
  HP_CreateFile(filename);

  int file_desc;
  HP_OpenFile(filename, &file_desc);

  Record record;
  srand(98765432); // Modified seed
  for (int id = 0; id < RECORDS_NUM; ++id)
  {
    record = randomRecord();
    HP_InsertEntry(file_desc, record);
  }
  return file_desc;
}

/* Performs the sorting phase of external merge sort algorithm on a file specified by 'file_desc', using chunks of size 'chunkSize' */
void sortPhase(int file_desc, int chunkSize)
{
  sort_FileInChunks(file_desc, chunkSize);
}

/* Performs the merge phase of the external merge sort algorithm using chunks of size 'chunkSize' and 'bWay' merging. The merge phase may be performed in more than one cycles. */
void mergePhases(int inputFileDesc, int chunkSize, int bWay, int *fileCounter)
{
  int outputFileDesc;
  int lastBlockID = HP_GetIdOfLastBlock(inputFileDesc);
  while (chunkSize <= lastBlockID)
  {
    outputFileDesc = nextOutputFile(fileCounter);
    merge(inputFileDesc, chunkSize, bWay, outputFileDesc);
    HP_CloseFile(inputFileDesc);
    chunkSize *= bWay;
    inputFileDesc = outputFileDesc;
  }
  HP_CloseFile(outputFileDesc);
}

/* Creates a sequence of heap files: out_modified0.db, out_modified1.db, ... and returns for each heap file its corresponding file descriptor. */
int nextOutputFile(int *fileCounter)
{
  char mergedFile[50];
  char tmp[] = "out_modified";
  sprintf(mergedFile, "%s%d.db", tmp, (*fileCounter)++);
  int file_desc;
  HP_CreateFile(mergedFile);
  HP_OpenFile(mergedFile, &file_desc);
  return file_desc;
}
