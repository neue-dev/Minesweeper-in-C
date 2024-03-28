/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-28 11:17:17
 * @ Modified time: 2024-03-29 02:17:54
 * @ Description:
 * 
 * A utility library for creating hash tables with a max size of 1 << 16 (2^16) elements.
 *    (Technically, we can have more because once collisions occur we implement
 *    linked lists to add more elements, but 2^16 is the most the array will expand to).
 *   
 * The array we use begins with 16 elements, but we double when the load factor goes above
 *    0.25 and the average linked list length goes above 1.333.
 */

#ifndef UTILS_HASHMAP_
#define UTILS_HASHMAP_

#include "./utils.math.h"
#include "./utils.string.h"
#include "./utils.types.h"

#include <stdlib.h>
#include <string.h>

typedef struct HashMapEntry HashMapEntry;
typedef struct HashMap HashMap;

#define HASHMAP_INITIAL_SIZE 1 << 4
#define HASHMAP_MAX_SIZE 1 << 16

/**
 * //
 * ////
 * //////    HashMapEntry class
 * ////////
 * ////////// 
*/

/**
 * The HashMapEntry class. This object represents the structure of the objects stored in a hashmap.
 * 
 * @class
*/
struct HashMapEntry {
  
  char sKey[STRING_KEY_MAX_LENGTH];   // The string we're feeding into the hash function
                                      // We don't really need this here, but this becomes handy when collisions occur and
                                      //    we want to iterate through a linked list to find an entry with a given key

  p_obj pObject;                      // A pointer to the actual data we want to index

  HashMapEntry *pNext;                // A pointer to the next entry, in case a collision occurs at this location

};

/**
 * Allocates memory for an instance of the HashMapEntry class.
 * 
 * @return	{ HashMapEntry * }  A pointer to the allocated memory.
*/
HashMapEntry *HashMapEntry_new() {
  HashMapEntry *pHashMapEntry = calloc(1, sizeof(*pHashMapEntry));
  return pHashMapEntry;
}

/**
 * Initializes an instance of the HashMapEntry class.
 * 
 * @param		{ HashMapEntry * }  this      A pointer to the instance to initialize.
 * @param   { char * }          sKey      The key associated with the entry.
 * @param   { p_obj }           pObject   A pointer to the object we're indexing.
 * @return	{ HashMapEntry * }				    A pointer to the initialized instance.
*/
HashMapEntry *HashMapEntry_init(HashMapEntry *this, char *sKey, p_obj pObject) {

  // Store the name
  strcpy(this->sKey, sKey);

  // Copy the reference to the object
  this->pObject = pObject;

  // Set the next in the linked list to NULL by default
  this->pNext = NULL;

  return this;
}

/**
 * Creates an initialized instance of the HashMapEntry class.
 * 
 * @param   { char * }          sKey      The key associated with the entry.
 * @param   { p_obj }           pObject   A pointer to the object we're indexing.
 * @return	{ HashMapEntry * }            A pointer to the newly created initialized instance.
*/
HashMapEntry *HashMapEntry_create(char *sKey, p_obj pObject) {
  return HashMapEntry_init(HashMapEntry_new(), sKey, pObject);
}

/**
 * Deallocates the memory of an instance of the HashMapEntry class.
 * 
 * @param		{ HashMapEntry * }  this	A pointer to the instance to deallocate.
*/
void HashMapEntry_kill(HashMapEntry *this) {
  free(this);
}

/**
 * //
 * ////
 * //////    HashMap class
 * ////////
 * ////////// 
*/

/**
 * The HashMap class. Helps us implement hashmaps (I honestly prefer to call them dictionaries) in C.
 * 
 * @class
*/
struct HashMap {
  
  int dEntryCount;          // How many entries atm
  int dEntrySlots;          // The number of slots in the hash map that are currently taken
  int dEntryMaxSlots;       // The maximum number of slots available for entries (subject to change when resizing)

  HashMapEntry **pEntries;  // Ah yes, a rather unsettling array of pointers

};

/**
 * Allocates memory for an instance of the HashMap class.
 * 
 * @return	{ HashMap * }		A pointer to the allocated memory.
*/
HashMap *HashMap_new() {
  HashMap *pHashMap = calloc(1, sizeof(*pHashMap));
  return pHashMap;
}

/**
 * Initializes an instance of the HashMap class.
 * 
 * @param		{ HashMap * }		this	A pointer to the instance to initialize.
 * @return	{ HashMap * }					A pointer to the initialized instance.
*/
HashMap *HashMap_init(HashMap *this) {
  int i;
  
  this->dEntryCount = 0;
  this->dEntryMaxSlots = HASHMAP_INITIAL_SIZE;

  // Allocate space for entry pointers
  this->pEntries = calloc(this->dEntryMaxSlots, sizeof(HashMapEntry *));

  // Set them all to NULL pointers for now
  for(i = 0; i < HASHMAP_INITIAL_SIZE; i++)
    this->pEntries[i] = NULL;

  return this;
}

/**
 * Creates an initialized instance of the HashMap class.
 * 
 * @return	{ HashMap * }		A pointer to the newly created initialized instance.
*/
HashMap *HashMap_create() {
  return HashMap_init(HashMap_new());
}

/**
 * Deallocates the memory of an instance of the HashMap class.
 * 
 * @param		{ HashMap * }		this	A pointer to the instance to deallocate.
*/
void HashMap_kill(HashMap *this) {
  int i;

  // We free all of the contents of the hashmap
  for(i = 0; i < this->dEntryMaxSlots; i++) {
    if(this->pEntries[i] != NULL) {
      if(this->pEntries[i]->pObject != NULL)
        free(this->pEntries[i]->pObject);
      
      HashMapEntry_kill(this->pEntries[i]);
    }
  }
  
  free(this);
}

/**
 * 
*/
/**
 * Resizes the hash map if the array starts to have too many collisions.
 * Note that our hash can only grow in size. Resize here only accomodates us
 *    when we have *too much stuff*, and not too little (after removing a lot 
 *    of entries).
 * 
 * PS I encountered some ferocious bugs trying to implement this routine.
 * 
 * @param		{ HashMap * }		this	A pointer to an instance of the HashMap class.
 */
void HashMap_resize(HashMap *this) {
  int i;
  HashMapEntry *pOldEntry, *pNewEntry;
  
  // Save the old entries
  HashMapEntry **pOldEntries = this->pEntries;

  // We won't resize if it's already too big
  if(this->dEntryMaxSlots >= HASHMAP_MAX_SIZE)
    return;

  // We're going to recompute the number of occupied slots
  this->dEntrySlots = 0;

  // Create a bigger allocation with twice the size
  this->dEntryMaxSlots <<= 1;
  this->pEntries = calloc(this->dEntryMaxSlots, sizeof(HashMapEntry *));

  // Set them all to NULL for now
  for(i = 0; i < this->dEntryMaxSlots; i++)
    this->pEntries[i] = NULL;

  // Generate new hashes based on the new size and add the entries array
  for(i = 0; i < this->dEntryMaxSlots >> 1; i++) {
    if(pOldEntries[i] != NULL) {
      pOldEntry = pOldEntries[i];

      // Go down the linked list
      do {
        int dNewHash = Math_hash(pOldEntry->sKey, this->dEntryMaxSlots);

        // If no collisions at this location
        if(this->pEntries[dNewHash] == NULL) {
          this->pEntries[dNewHash] = pOldEntry;
          this->dEntrySlots++;
        
        // If there's something there already
        } else {
          pNewEntry = this->pEntries[dNewHash];

          // Get the last entry in the chain
          while(pNewEntry->pNext != NULL)
            pNewEntry = pNewEntry->pNext;

          pNewEntry->pNext = pOldEntry;
        }

        // We're reconstructing the linked lists too
        if(pOldEntry->pNext != NULL) {
          pOldEntries[i] = pOldEntry->pNext;
          pOldEntry->pNext = NULL;
        } else {
          pOldEntries[i] = NULL;  
        }

        // Increment the pointer
        pOldEntry = pOldEntries[i];
      } while(pOldEntry != NULL);
    }
  }

  // Garbage collection
  free(pOldEntries);
}

/**
 * Adds a new entry into the hash map.
 * 
 * @param		{ HashMap * }		this	    A pointer to an instance of the HashMap class.
 * @param   { char * }      sKey      The key of the entry we want.
 * @param   { p_obj }       pObject   A reference to the actual object we're going to store.
 */
void HashMap_add(HashMap *this, char *sKey, p_obj pObject) {
  int dLoadFactor, dHash;
  HashMapEntry *pEntry, *pNewEntry;

  // Get the pointer at the hash location create a new entry
  pNewEntry = HashMapEntry_create(sKey, pObject);
  dHash = Math_hash(sKey, this->dEntryMaxSlots);
  pEntry = this->pEntries[dHash];

  // No collisions
  if(pEntry == NULL) {
    this->pEntries[dHash] = pNewEntry;
    this->dEntrySlots++;

  // If there's a collision, start a linked list there!
  // (assuming the key isnt a duplicate)
  } else {
    
    // While there's smth next in the chain
    while(pEntry->pNext != NULL) {

      // If there's a duplicate
      if(!strcmp(pEntry->sKey, sKey)) {
        free(pNewEntry);
        return;
      }

      // Check next
      pEntry = pEntry->pNext;
    }

    // Append to the linked list
    pEntry->pNext = pNewEntry;
  }

  // A new entry was appended
  this->dEntryCount++;

  // Note the * 1000 here is because they're both ints
  // I didn't wanna have to typecast anymore, so we check if this value 1250  instead.
  dLoadFactor = this->dEntryCount * 1000 / this->dEntrySlots;
  
  // We resize if too much sht piles up
  if(dLoadFactor > 1333 && this->dEntrySlots > 0.25 * this->dEntryMaxSlots)
    HashMap_resize(this);
}

/**
 * Since we're abstracting how this function works, we return the actual object
 *    stored by the hash map at a certain location.
 * This function returns NULL when the key is currently not in the hashmap.
 * 
 * @param		{ HashMap * }		this	A pointer to an instance of the HashMap class.
 * @param   { char * }      sKey  The key to look for in the hashmap.
 * @return  { p_obj }             A reference to the object in the entry or NULL if the key does not exist.    
 */
p_obj HashMap_get(HashMap *this, char *sKey) {
  int dHash;
  HashMapEntry *pEntry;

  // Compute hash and get location
  dHash = Math_hash(sKey, this->dEntryMaxSlots);
  pEntry = this->pEntries[dHash];

  // While we haven't found the right match
  while(pEntry != NULL) {
    if(!strcmp(pEntry->sKey, sKey))
      return pEntry->pObject;
    
    pEntry = pEntry->pNext;
  }

  // The entry wasn't found
  return NULL;
}

/**
 * This function changes what object the entry with a certain key points to.
 * Note that the original object must be freed first; otherwise, a memory leak
 *    will occur.
 * 
 * @param		{ HashMap * }		this	    A pointer to an instance of the HashMap class.
 * @param   { char * }      sKey      The key to look for in the hashmap.
 * @param   { p_obj }       pObject   A reference to the object to put in the entry.
 */
void HashMap_set(HashMap *this, char *sKey, p_obj pObject) {
  int dHash;
  HashMapEntry *pEntry;

  // Compute hash and get location
  dHash = Math_hash(sKey, this->dEntryMaxSlots);
  pEntry = this->pEntries[dHash];

  // While we haven't found the right match
  while(pEntry != NULL) {
    
    // It's a match
    if(!strcmp(pEntry->sKey, sKey)) {

      // Set the new object
      pEntry->pObject = pObject;

      return;
    }
    
    // Increment the pointer
    pEntry = pEntry->pNext;
  }
}

/**
 * Removes an entry from the hashmap.
 * Note that this also frees the object associated with the entry.
 * 
 * @param		{ HashMap * }		this	A pointer to an instance of the HashMap class.
 */
void HashMap_del(HashMap *this, char *sKey) {
  int dHash;
  HashMapEntry *pEntry, *pPrevEntry;

  // Get the location of the key
  dHash = Math_hash(sKey, this->dEntryMaxSlots);
  pPrevEntry = this->pEntries[dHash];
  pEntry = this->pEntries[dHash];

  // Key doesn't exist
  if(pEntry == NULL)
    return;

  // There is a terminating condition at the bottom (return statement)
  while(1) {

    // The key was found
    if(!strcmp(pEntry->sKey, sKey)) {

      // It's the first node in the linked list
      if(pEntry == pPrevEntry)
        this->pEntries[dHash] = pEntry->pNext;

      // Bridge the gap in the linked list
      // Even if it's NULL it's okay
      else
        pPrevEntry->pNext = pEntry->pNext;

      // Delete the entry and its object
      if(pEntry->pObject != NULL)
        free(pEntry->pObject);
      free(pEntry);
      
      return;
    }
    
    // The key was also not found
    if(pEntry->pNext == NULL)
      return;

    // Move to the next in the linked list
    pPrevEntry = pEntry;
    pEntry = pEntry->pNext;
  }
}

/**
 * Returns all the keys in the hashmap.
 * It is not advisable to call this function a lot, unless necessary (such as deleting all elements
 *    in the hashmap during clean up); this function defeats the purpose of using a hashmap because
 *    it iterates through all elements.
 * This function assumes the target array is big enough to hold all keys.
 * 
 * @param		{ HashMap * }		this	      A pointer to an instance of the HashMap class.
 * @param   { char *[] }    sKeyArray   Where the keys will be outputted to.
 */
void HashMap_getKeys(HashMap *this, char *sKeyArray[]) {
  int i, dOutputIndex = 0;
  HashMapEntry *pEntry;

  // Go through entire array
  for(i = 0; i < this->dEntryMaxSlots; i++) {
    pEntry = this->pEntries[i];

    // Go down the linked list
    while(pEntry != NULL) {
      sKeyArray[dOutputIndex++] = String_create(pEntry->sKey);
      pEntry = pEntry->pNext;
    }
  }
}

#endif