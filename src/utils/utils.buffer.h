/**
 * Implements a string buffer class which allows us to "paint" with strings before
 *    printing the final outcome with a single print call to the standard output stream.
*/

#ifndef UTILS_BUFFER
#define UTILS_BUFFER

#include <string.h>
#include <stdio.h>

#define BUFFER_MAX_LENGTH 1024

/**
 * The buffer class holds an array of strings which we print in bulk later on.
 * @class
*/
typedef struct Buffer {
  char *sBlocksArray[BUFFER_MAX_LENGTH];  // Stores all the blocks of text the buffer has
  int dLength;                            // The current size of the buffer array
} Buffer;

#endif