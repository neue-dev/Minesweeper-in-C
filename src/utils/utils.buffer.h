/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-20 02:22:07
 * @ Modified time: 2024-02-20 11:44:12
 * @ Description:
 *   
 * A buffer class that can help us create blocks of text before printing them.
 */

#ifndef UTILS_BUFFER
#define UTILS_BUFFER

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define BUFFER_MAX_WIDTH 1024
#define BUFFER_MAX_HEIGHT 1024

/**
 * Buffer class yes
 * @class
*/
typedef struct Buffer {
  int dLength;                      // The current length of the buffer (in lines).
  int dWidth;                       // The maximum width of the buffer (in characters).
  int dHeight;                      // The maximum height of the buffer (in lines).

  char *sArray[BUFFER_MAX_HEIGHT];  // An array that stores the string contents of each of the lines in the buffer.
  int dArray[BUFFER_MAX_HEIGHT];    // An array that stores the lengths of each of the lines in the buffer.
} Buffer;

/**
 * Allocates memory for a new instance of the buffer class.
 * 
 * @return  { Buffer * }  A pointer to the location in memory of the buffer instance.
*/
Buffer *Buffer_new() {
  Buffer *pBuffer = calloc(1, sizeof(*pBuffer));
  return pBuffer;
}

/**
 * Initializes an instance of the buffer class.
 * Sets its current length to 0.
 * 
 * @param   { Buffer * }  this      A pointer to the instance we're going to init.
 * @param   { Buffer * }  dWidth    The width of each of the buffer lines.
 * @param   { Buffer * }  dHeight   The height of the buffer.
 * @return  { Buffer * }            The pointer to the initialized instance.
*/
Buffer *Buffer_init(Buffer *this, int dWidth, int dHeight) {
  this->dLength = 0;

  this->dWidth = dWidth < BUFFER_MAX_WIDTH ? dWidth : BUFFER_MAX_WIDTH;
  this->dHeight = dHeight < BUFFER_MAX_HEIGHT ? dHeight : BUFFER_MAX_HEIGHT;

  return this;
}

/**
 * Creates an initialized instance of the buffer class.
 * Sets its current length to 0.
 * 
 * @param   { Buffer * }  dWidth    The width of each of the buffer lines.
 * @param   { Buffer * }  dHeight   The height of the buffer.
 * @return  { Buffer * }  The pointer to the initialized instance.
*/
Buffer *Buffer_create(int dWidth, int dHeight) {
  return Buffer_init(Buffer_new(), dWidth, dHeight);
}

/**
 * Frees the memory associated with an instance of the buffer class.
 * 
 * @param   { Buffer * }  this  The instance to be freed from memory.
*/
void Buffer_kill(Buffer *this) {
  free(this);
}

/**
 * The function appends a string to the array of the buffer and doesn't count format specifiers
 *    in its length.
 * Note the format specifiers should only be used for escape sequences.
 * Woah this actually worked?
 * 
 * @param   { Buffer * }  this    The buffer to be modified.
 * @param   { char * }    sEntry  The format string to use.
 * @param   { char * }    ...     Any ANSI escape sequences we wish to embed into the format string.
*/
void Buffer_append(Buffer *this, char *sEntry, ...) {
  int i = 0;
  int dEntryLen = 0;

  // Because our function is variadic, ie, has a variable number of arguments
  va_list pArg;
  va_start(pArg, sEntry);

  // Can't have too many lines now can we?
  if(this->dLength >= this->dHeight)
    return;

  // This counts the length of the string excluding format basic specifiers.
  // By basic format specifiers we mean %n; format specifiers with a . or 
  // other numbers will not be excluded.
  do {
    if(sEntry[i] == '%') {
      // Do nothing

    } else {
      if(i > 0) {
        if(sEntry[i - 1] != '%') {
          dEntryLen++;
        }
      } else {
        dEntryLen++;
      }
    }
  
  // Iterate through the entire string
  } while(sEntry[++i]);

  printf("%d\n", dEntryLen);

  // Copy the format string first
  char *sFormattedEntry = calloc(this->dWidth, sizeof(char));
  vsnprintf(sFormattedEntry, this->dWidth, sEntry, pArg);

  // Final save it into the buffer
  this->sArray[this->dLength] = sFormattedEntry;
  this->dArray[this->dLength] = dEntryLen;
  this->dLength++;
}

/**
 * Outputs the buffer to the screen as one massive blob.
 * 
*/
void Buffer_print(Buffer *this) {
  int i;
  int dMaxSize = BUFFER_MAX_HEIGHT * BUFFER_MAX_WIDTH;

  // Note that we use max height and max width instead of the instance's dimensions because
  //    we need extra space for any ANSI escape sequences that might be stored in the buffer.
  char *sBlob = calloc(dMaxSize, sizeof(char));

  // Iterate through the lines
  for(i = 0; i < this->dLength; i++) {
    if(strlen(sBlob) + strlen(this->sArray[i]) < dMaxSize)
      strcat(sBlob, this->sArray[i]);

    if(strlen(sBlob) + 1 < dMaxSize)
      strcat(sBlob, "\n");
  }

  printf(sBlob);
}

#endif
