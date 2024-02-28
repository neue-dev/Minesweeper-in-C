/**
 * @ Author: Mo David
 * @ Create Time: 2024-02-28 11:17:17
 * @ Modified time: 2024-02-28 11:56:58
 * @ Description:
 * 
 * A utility library for creating hash tables with a max size of 1 << 16 (2^16) elements.
 *    (Technically, we can have more because once collisions occur we implement
 *    linked lists to add more elements, but 2^16 is the most the array will expand to).
 *   
 * The array we use begins with 1024 elements, but we double when the load factor goes above
 *    0.7, as is recommended by most sources.
 */

#ifndef UTILS_HASHMAP_
#define UTILS_HASHMAP_

#include "./utils.math.h"
#include "./utils.types.h"

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
typedef struct HashMapEntry {
  
  p_obj pObject;  // A pointer to the actual data we want to index

} HashMapEntry;

/**
 * Allocates memory for an instance of the HashMapEntry class.
 * 
 * @return	{ HashMapEntry * }		A pointer to the allocated memory.
*/
HashMapEntry *HashMapEntry_new() {
  HashMapEntry *pHashMapEntry = calloc(1, sizeof(*pHashMapEntry));
  return pHashMapEntry;
}

/**
 * Initializes an instance of the HashMapEntry class.
 * 
 * @param		{ HashMapEntry * }		this	A pointer to the instance to initialize.
 * @return	{ HashMapEntry * }					A pointer to the initialized instance.
*/
HashMapEntry *HashMapEntry_init(HashMapEntry *this) {

  return this;
}

/**
 * Creates an initialized instance of the HashMapEntry class.
 * 
 * @return	{ HashMapEntry * }		A pointer to the newly created initialized instance.
*/
HashMapEntry *HashMapEntry_create() {
  return HashMapEntry_init(HashMapEntry_new());
}

/**
 * Deallocates the memory of an instance of the HashMapEntry class.
 * 
 * @param		{ HashMapEntry * }		this	A pointer to the instance to deallocate.
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
typedef struct HashMap {
  HashMapEntry **pEntries;  // Ah yes, a rather unsettling array of pointers
} HashMap;

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
  free(this);
}


#endif