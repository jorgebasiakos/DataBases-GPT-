#include <merge.h>
#include <stdio.h>
#include "chunk.h"
#include <string.h>

/*Δημιουργεί έναν ChunkIterator για αποτελεσματική διέλευση συρμων μέσα σε ένα αρχείο, που καθορίζεται από τον file descriptor.
Ο iterator έχει διαμορφωθεί με ένα καθορισμένο εύρος απο μπλοκς (συνήθως ξεκινώντας από το μπλοκ 1), μαζί με το μέγεθος
για κάθε συρμο και τον μέγιστο αριθμό εγγραφών σε κάθε μπλοκ.*/
CHUNK_Iterator CHUNK_CreateIterator(int fileDesc, int blocksInChunk)
{
  CHUNK_Iterator iterator;
  iterator.file_desc = fileDesc;
  iterator.current = 1; // Assuming it starts from block 1
  iterator.lastBlocksID = HP_GetIdOfLastBlock(fileDesc);
  iterator.blocksInChunk = blocksInChunk;

  return iterator;
}

int calculateRecordsInChunk(int file_desc, int from_BlockId, int to_BlockId)
{
  int recordsCount = 0;

  // Iterate through blocks in the specified chunk
  for (int blockId = from_BlockId; blockId <= to_BlockId; ++blockId)
  {
    // Retrieve the number of records in the current block
    int currentBlockRecords = HP_GetRecordCounter(file_desc, blockId);

    if (currentBlockRecords > 0)            //++
    {
      // Add the count to the total records count
      recordsCount += currentBlockRecords;
    }
  }

  return recordsCount;
}

int calculateBlocksInChunk(int from_BlockId, int to_BlockId)
{
  // Calculate the number of blocks in the chunk
  return to_BlockId - from_BlockId + 1;
}

// Ανακτά το επόμενο CHUNK στην ακολουθία σύμφωνα με τον παρεχόμενο CHUNK_Iterator
int CHUNK_GetNext(CHUNK_Iterator *iterator, CHUNK *chunk)
{
  // Check if there are more chunks to iterate
  if (iterator->current <= iterator->lastBlocksID)
  {
    // Update the chunk's records and blocks count
    chunk->recordsInChunk = calculateRecordsInChunk(iterator->file_desc, iterator->current, iterator->current + iterator->blocksInChunk - 1);
    chunk->blocksInChunk = calculateBlocksInChunk(iterator->current, iterator->current + iterator->blocksInChunk - 1);
    chunk->from_BlockId = iterator->current;        //++...
    chunk->to_BlockId = iterator->current + iterator->blocksInChunk - 1;
    chunk->file_desc = iterator->file_desc;       

    // Move to the next chunk
    iterator->current += iterator->blocksInChunk;

    return 0; // Success
  }
  else
  {
    return -1; // No more chunks
  }
}

/*Ανακτά την i την εγγραφή από έναν CHUNK με μπλοκς σε ένα αρχείο σωρού. Επιστρέφει 0 εάν είναι επιτυχής,
συμπλήρωση της παραμέτρου «record». διαφορετικά, -1. Προϋποθέτει τη διαδοχική διαταξη των εγγραφών εντός του συρμου*/
int CHUNK_GetIthRecordInChunk(CHUNK *chunk, int i, Record *record)
{
  // Check if the given index is within the bounds of recordsInChunk
  if (i >= 0 && i < chunk->recordsInChunk)
  {
    // Calculate the block and record index based on the ith record
    int blockIndex = i / HP_GetMaxRecordsInBlock(chunk->file_desc);
    int recordIndexInBlock = i % HP_GetMaxRecordsInBlock(chunk->file_desc);

    // Calculate the actual block ID
    int blockId = chunk->from_BlockId + blockIndex;

    // Retrieve the record from the specified block and record index
    if (HP_GetRecord(chunk->file_desc, blockId, recordIndexInBlock, record) == 0)
    {
      HP_Unpin(chunk->file_desc, blockId);    //++
      return 0; // Success
    }
    else
    {
      return -1; // Failed to retrieve the record
    }
  }
  else        
  {
    return -1; // Index out of bounds
  }
}

/*ενημερώνει την n-οστη εγγραφή σε έναν συρμο. Επιστρέφει 0 εάν είναι επιτυχής. -1 εάν αποτύχει.
Διευκολύνει στην αποτελεσματικη και ελεγχόμενη ενημερωση στα πλαισια ενος συρμου*/
int CHUNK_UpdateIthRecord(CHUNK *chunk, int i, Record record)
{
  // Check if the given index is within the bounds of recordsInChunk
  if (i >= 0 && i < chunk->recordsInChunk)
  {
    // Calculate the block and record index based on the ith record
    int blockIndex = i / HP_GetMaxRecordsInBlock(chunk->file_desc);
    int recordIndexInBlock = i % HP_GetMaxRecordsInBlock(chunk->file_desc);

    // Calculate the actual block ID
    int blockId = chunk->from_BlockId + blockIndex;

    // Update the record in the specified block and record index
    if (HP_UpdateRecord(chunk->file_desc, blockId, recordIndexInBlock, record) == 0)
    {
      return 0; // Success
    }
    else
    {
      return -1; // Failed to update the record
    }
  }
  else
  {
    return -1; // Index out of bounds
  }
}

/* This function is used to print the records within a chunk.*/
void CHUNK_Print(CHUNK chunk)
{
  printf("Chunk Information:\n");
  printf("File Descriptor: %d\n", chunk.file_desc);
  printf("From Block ID: %d\n", chunk.from_BlockId);
  printf("To Block ID: %d\n", chunk.to_BlockId);
  printf("Records in Chunk: %d\n", chunk.recordsInChunk);
  printf("Blocks in Chunk: %d\n", chunk.blocksInChunk);

  // Print the records within the chunk
  for (int i = 0; i < chunk.recordsInChunk; ++i)
  {
    Record record;
    if (CHUNK_GetIthRecordInChunk(&chunk, i, &record) == 0)
    {
      printf("Record %d: ID=%d, Name=%s, Surname=%s, City=%s\n", i + 1, record.id, record.name, record.surname, record.city);
    }
    else
    {
      printf("Error retrieving record %d\n", i + 1);
    }
  }
}

// Δημιουργεί έναν iterator εγγραφής για αποτελεσματική διέλευση εντός ενός CHUNK
CHUNK_RecordIterator CHUNK_CreateRecordIterator(CHUNK *chunk)
{
  CHUNK_RecordIterator iterator;
  iterator.chunk = *chunk;
  iterator.currentBlockId = chunk->from_BlockId;
  iterator.cursor = 0;
  return iterator;
}

// λήψη της επόμενης εγγραφής από τον iterator
int CHUNK_GetNextRecord(CHUNK_RecordIterator *iterator, Record *record)
{
  if (iterator == NULL || record == NULL)
  {
    // Handle invalid arguments
    return -1;
  }

  CHUNK *chunk = &(iterator->chunk);
  int currentBlockId = iterator->currentBlockId;
  int cursor = iterator->cursor;

  // Check if we need to move to the next block
  if (cursor % (BF_BLOCK_SIZE / sizeof(Record)) == 0 && cursor != 0)  //++
  {
    // Move to the next block
    currentBlockId++;
  }

  // Check if we have reached the end of the chunk
  if (currentBlockId > chunk->to_BlockId)
  {
    // No more records to retrieve
    return -1;
  }

  // Retrieve the record from the current block and cursor
  if (CHUNK_GetIthRecordInChunk(chunk, cursor, record) != 0)
  {
    // Failed to retrieve the record
    return -1;
  }

  // Update the iterator
  iterator->currentBlockId = currentBlockId;
  iterator->cursor = cursor + 1;

  return 0;
}
