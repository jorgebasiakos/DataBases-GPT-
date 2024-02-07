#ifndef CHUNK_H
#define CHUNK_H

#include <stdio.h>
#include <stdlib.h>
#include "hp_file.h"



/* Represents a chunk of records in a file, defining the file descriptor, starting and ending block IDs, and the 
counts of records and blocks in the chunk. Useful for managing and sorting records within specific chunks. 
αντιπροσωπεύει έναν συρμο με εγγραφες σε ένα αρχείο, ορίζοντας τον περιγραφέα αρχείου, τα IDs του 1ου και του τελευταιου μπλοκ
και τον αριθμο των εγγραφων και των μπλοκς στον συρμο. Χρήσιμο για τη διαχείριση και ταξινόμηση εγγραφών σε συγκεκριμένους συρμους.*/
typedef struct {
    int file_desc;
    int from_BlockId;
    int to_BlockId;
    int recordsInChunk;
    int blocksInChunk;
} CHUNK;

/* Represents an iterator for traversing chunks within a file, storing the file descriptor, current block, last 
block ID, and the total number of blocks in each chunk. Useful for efficiently iterating over file chunks.
Αντιπροσωπεύει έναν iterator για τη διέλευση συρμων μέσα σε ένα αρχείο, την αποθήκευση του file descriptor, το τρέχον μπλοκ, 
το τελευταίο μπλοκ ID και τον συνολικό αριθμό μπλοκς σε κάθε συρμο. Χρήσιμο για αποτελεσματικο iterating σε συρμους αρχείων*/
typedef struct {
    int file_desc;
    int current;
    int lastBlocksID;
    int blocksInChunk;
} CHUNK_Iterator;

/* Creates a ChunkIterator for efficient traversal of chunks within a file, specified by the file descriptor. 
The iterator is configured with a defined range of blocks (usually starting from block 1), along with the size 
of each chunk and the maximum number of records in each block.
Δημιουργεί έναν ChunkIterator για αποτελεσματική διέλευση συρμων μέσα σε ένα αρχείο, που καθορίζεται από τον file descriptor.
Ο iterator έχει διαμορφωθεί με ένα καθορισμένο εύρος απο μπλοκς (συνήθως ξεκινώντας από το μπλοκ 1), μαζί με το μέγεθος
για κάθε συρμο και τον μέγιστο αριθμό εγγραφών σε κάθε μπλοκ.*/
CHUNK_Iterator CHUNK_CreateIterator(int fileDesc, int blocksInChunk);

/* Retrieves the next CHUNK in the sequence as per the provided CHUNK_Iterator. */
int CHUNK_GetNext(CHUNK_Iterator *iterator,CHUNK* chunk);

/* Retrieves the ith record from a CHUNK of blocks in a heap file. Returns 0 if successful, 
populating the 'record' parameter; otherwise, -1. Assumes sequential ordering of records within the chunk
Ανακτά την i την εγγραφή από έναν CHUNK με μπλοκς σε ένα αρχείο σωρού. Επιστρέφει 0 εάν είναι επιτυχής,
συμπλήρωση της παραμέτρου «record». διαφορετικά, -1. Προϋποθέτει τη διαδοχική διαταξη των εγγραφών εντός του συρμου.*/
int CHUNK_GetIthRecordInChunk(CHUNK* chunk,  int i, Record* record);//

/* Updates the ith record in a chunk. Returns 0 if successful; -1 if unsuccessful. 
Facilitates efficient and controlled updates within a chunk.*/
/*ενημερώνει την n-οστη εγγραφή σε έναν συρμο. Επιστρέφει 0 εάν είναι επιτυχής. -1 εάν αποτύχει.
Διευκολύνει στην αποτελεσματικη και ελεγχόμενη ενημερωση στα πλαισια ενος συρμου*/
int CHUNK_UpdateIthRecord(CHUNK* chunk,  int i, Record record);

/* This function is used to print the records within a chunk.*/
void CHUNK_Print(CHUNK chunk);//

/* Iterates through records in a CHUNK, encapsulating the id of the current block and a cursor in that block.
κανει iterate διαμέσω των εγγραφών σε έναν CHUNK, ενσωματώνοντας το ID του τρέχοντος μπλοκ και έναν δρομέα σε αυτό το μπλοκ. */
typedef struct CHUNK_RecordIterator {
    CHUNK chunk;
    int currentBlockId;
    int cursor;
} CHUNK_RecordIterator;

/* Creates a record iterator for efficient traversal within a CHUNK. 
Δημιουργεί έναν iterator εγγραφής για αποτελεσματική διέλευση εντός ενός CHUNK*/
CHUNK_RecordIterator CHUNK_CreateRecordIterator(CHUNK *chunk);


/* Function to get the next record from the iterator.
λήψη της επόμενης εγγραφής από τον iterator */
int CHUNK_GetNextRecord(CHUNK_RecordIterator *iterator,Record* record);

#endif  // MY_HEADER_H