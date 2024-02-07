#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bf.h"
#include "hp_file.h"
#include "record.h"
#include "sort.h"
#include "merge.h"
#include "chunk.h"

/* Determines if two records should be swapped during sorting, returning true if the order needs adjustment.*/
bool shouldSwap(Record *rec1, Record *rec2)
{
  // Compare first by name
  int nameCompare = strcmp(rec1->name, rec2->name);
  if (nameCompare != 0)
  {
    return (nameCompare > 0);
  }
  else
  {
    // If names are the same, compare by surname
    return (strcmp(rec1->surname, rec2->surname) > 0);   //++
  }
}

/*Ταξινομεί τα περιεχόμενα ενός αρχείου που προσδιορίζεται από το 'file_desc' σε συρμους, όπου κάθε συρμος περιέχει μπλοκς = 'numBlocksInChunk'.
Η ταξινόμηση εκτελείται επιτόπου μέσα σε κάθε συρμο, χρησιμοποιώντας έναν κατάλληλο αλγόριθμο ταξινόμησης.*/
void sort_FileInChunks(int file_desc, int numBlocksInChunk)
{
  // Calculate the total number of records in a chunk
  int recordsPerChunk = numBlocksInChunk * (BF_BLOCK_SIZE / sizeof(Record));

  // Allocate memory for the records
  Record *records = malloc(recordsPerChunk * sizeof(Record));
  if (records == NULL)
  {
    // Handle memory allocation error
    exit(EXIT_FAILURE);
  }

  // printf("dd:%d\n", HP_GetIdOfLastBlock(file_desc));

  // Iterate over chunks
  for (int i = 1; i <= HP_GetIdOfLastBlock(file_desc); i += numBlocksInChunk)
  {
    // Create a CHUNK structure or pointer and populate it with the chunk information
    CHUNK currentChunk;
    currentChunk.file_desc = file_desc;
    currentChunk.from_BlockId = i;
    currentChunk.to_BlockId = i + numBlocksInChunk - 1;
    // Fill records with data from chunks
    int recordIndex = 0;
    // printf("%d", HP_GetIdOfLastBlock(file_desc));

    for (int j = currentChunk.from_BlockId; j <= currentChunk.to_BlockId; j++)
    {
      // Read records from the block and copy them to the records array
      int kk = HP_GetIdOfLastBlock(file_desc);        //++...
      if (j < kk)
      {
        for (int k = 0; k < (BF_BLOCK_SIZE / sizeof(Record)); k++)
        {
          if (k < HP_GetRecordCounter(file_desc, j))
          {
            if (HP_GetRecord(file_desc, j, k, &records[recordIndex]) == -1)
            {
              // Handle error or break if there are no more records
              break;
            }
            recordIndex++;
          }
        }
      }
    }

    // Sort the records using the sort_Chunk function
    sort_Chunk(&currentChunk);
  }

  free(records); // Don't forget to free allocated memory
}

/* Sorts records within a CHUNK in ascending order based on the name and surname of each person. */
void sort_Chunk(CHUNK *chunk)
{
  int recordsCount = 0;

  // Calculate the total number of records in the chunk
  for (int blockId = chunk->from_BlockId; blockId <= chunk->to_BlockId; ++blockId)
  {
    int cnt = HP_GetRecordCounter(chunk->file_desc, blockId); //++...
    if (cnt > 0)
    {
      recordsCount += cnt;
    }
    // printf("cnt=%d, total=%d\n", cnt, recordsCount);
  }

  // Allocate memory for the records in the chunk
  Record *records = (Record *)malloc(recordsCount * sizeof(Record));

  if (records == NULL)
  {
    // Handle memory allocation error
    exit(EXIT_FAILURE);
  }

  // Fill the records array with data from the chunk
  int recordIndex = 0;

  for (int blockId = chunk->from_BlockId; blockId <= chunk->to_BlockId; ++blockId)
  {
    int blockRecords = HP_GetRecordCounter(chunk->file_desc, blockId);

    for (int recordIndexInBlock = 0; recordIndexInBlock < blockRecords; ++recordIndexInBlock)
    {
      // Retrieve the record from the block
      if (HP_GetRecord(chunk->file_desc, blockId, recordIndexInBlock, &records[recordIndex]) != 0)
      {
        // Handle error
        free(records);
        exit(EXIT_FAILURE);
      }

      recordIndex++;
    }
  }

  // Sort the records using the shouldSwap function (bubble sort for simplicity)
  for (int i = 0; i < recordsCount - 1; i++)
  {
    for (int j = 0; j < recordsCount - i - 1; j++)
    {
      if (shouldSwap(&records[j], &records[j + 1]))
      {
        // Swap records if necessary
        Record temp = records[j];
        records[j] = records[j + 1];
        records[j + 1] = temp;
      }
    }
  }

  // Update the records in the chunk with the sorted records
  recordIndex = 0;

  for (int blockId = chunk->from_BlockId; blockId <= chunk->to_BlockId; ++blockId)
  {
    int blockRecords = HP_GetRecordCounter(chunk->file_desc, blockId);

    if (blockRecords > 0)     //++
    {
      for (int recordIndexInBlock = 0; recordIndexInBlock < blockRecords; ++recordIndexInBlock)
      {
        // Update the record in the block
        if (recordIndex < recordsCount)   //++
        {
          if (HP_UpdateRecord(chunk->file_desc, blockId, recordIndexInBlock, records[recordIndex]) != 0)
          {
            // Handle error
            free(records);
            exit(EXIT_FAILURE);
          }

          recordIndex++;
        }
      }
      HP_Unpin(chunk->file_desc, blockId);    //++
    }
  }

  free(records);
}