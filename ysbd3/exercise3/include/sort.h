#ifndef SORT_H
#define SORT_H


#include <stdbool.h>
#include "chunk.h"

/* Determines if two records should be swapped during sorting, returning true if the order needs adjustment.*/
bool shouldSwap(Record* rec1,Record* rec2);

/* Sorts the contents of a file identified by 'file_desc' in chunks, where each chunk contains 'numBlocksInChunk' blocks.
The sorting is performed in-place within each chunk, using an appropriate sorting algorithm.
Ταξινομεί τα περιεχόμενα ενός αρχείου που προσδιορίζεται από το 'file_desc' σε συρμους, όπου κάθε συρμος περιέχει μπλοκς = 'numBlocksInChunk'.
Η ταξινόμηση εκτελείται επιτόπου μέσα σε κάθε συρμο, χρησιμοποιώντας έναν κατάλληλο αλγόριθμο ταξινόμησης.*/
void sort_FileInChunks(int file_desc, int numBlocksInChunk);

/* Sorts records within a CHUNK in ascending order based on the name and surname of each person. */
void sort_Chunk(CHUNK* chunk);

#endif 
