#include <merge.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "merge.h"

int compareRecords(const Record *rec1, const Record *rec2)
{
  // Compare first by name
  int nameCompare = strcmp(rec1->name, rec2->name);
  //printf("%s - %s", rec1->name, rec2->name);

  if (nameCompare != 0)
  {
    return nameCompare;
  }
  else
  {
    // If names are the same, compare by surname
    int surnameCompare = strcmp(rec1->surname, rec2->surname);
    return surnameCompare;
  }
}

/*Συγχωνεύει κάθε b συρμους μεγέθους chunkSize από το αρχείο εισόδου στο καθορισμένο αρχείο εξόδου.
Η συνάρτηση λαμβάνει τους file descriptors εισόδου και εξόδου, το μέγεθος του συρμου και τον αριθμό των τμημάτων
προς συγχώνευση. Θα πρέπει να χρησιμοποιεί εσωτερικά ένα CHUNK_Iterator και ένα CHUNK_RecordIterator.*/
void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc)
{
  int totalChunks = ceil(((double)HP_GetIdOfLastBlock(input_FileDesc)) / ((double)chunkSize));    //++...

  // Create a CHUNK_Iterator for efficient traversal of chunks within the input file
  CHUNK_Iterator inputIterator = CHUNK_CreateIterator(input_FileDesc, chunkSize);

  for(int j=0; j<totalChunks; j+=bWay){
    int chunksToMerge = ((totalChunks-j) < bWay)? totalChunks-j : bWay;

    // Create a CHUNK_RecordIterator for efficient traversal within a CHUNK
    CHUNK_RecordIterator recordIterator[chunksToMerge];
    CHUNK currentChunk[chunksToMerge];
    Record* currentRecord[chunksToMerge];

    // Iterate through b chunks in each iteration
    for (int i = 0; i < chunksToMerge; i++)
    {
      // Get the next CHUNK from the input file
      if (CHUNK_GetNext(&inputIterator, &(currentChunk[i])) == -1)
      {
        // Handle error or break from the loop if there are no more chunks
        printf("could not get chunk\n");
        break;
      }

      // Initialize the CHUNK_RecordIterator for the current CHUNK
      recordIterator[i] = CHUNK_CreateRecordIterator(&currentChunk[i]);

      currentRecord[i] = malloc(sizeof(Record));
      if (currentRecord[i] == NULL)
      {
        fprintf(stderr, "Memory allocation error\n");   
        exit(EXIT_FAILURE);
      }
      
      // Load the first record from the chunk
      if (CHUNK_GetNextRecord(&(recordIterator[i]), currentRecord[i]) == -1)
      {
        fprintf(stderr, "Error reading record from chunk\n");
        exit(EXIT_FAILURE);
      }
    } 

    while (true)
    {
      // Find the smallest record among the current records from each chunk
      int smallestIndex = -1;
      for (int k = 0; k < chunksToMerge; k++)
      {
        if (currentRecord[k] != NULL)
        {
          if (smallestIndex == -1 || compareRecords(currentRecord[k], currentRecord[smallestIndex]) < 0)
          {
            smallestIndex = k;
          }
        }
      }

      // Check if all chunks are exhausted
      if (smallestIndex == -1)
      {
        break; // All chunks are exhausted
      }

      // Write the smallest record to the output file
      if (HP_InsertEntry(output_FileDesc, *currentRecord[smallestIndex]) == -1)
      {
        fprintf(stderr, "Error writing record to output file\n");
        exit(EXIT_FAILURE);
      }

      // Read the next record from the chunk that provided the smallest record
      if (CHUNK_GetNextRecord(&(recordIterator[smallestIndex]), currentRecord[smallestIndex]) == -1)
      {
        // Chunk is exhausted, set the record pointer to NULL
        free(currentRecord[smallestIndex]);
        currentRecord[smallestIndex] = NULL;
      }
    }

    // Clean up resources
    for (int k = 0; k < chunksToMerge; k++)
    {
      // Release resources or perform cleanup for each chunk
      free(currentRecord[k]);
    }

  }
}
