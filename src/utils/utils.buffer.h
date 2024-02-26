/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-20 02:22:07
 * @ Modified time: 2024-02-26 18:11:40
 * @ Description:
 *   
 * A buffer class that can help us create blocks of text before printing them.
 */

#ifndef UTILS_BUFFER_
#define UTILS_BUFFER_

#include "./utils.io.h"
#include "./utils.string.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define BUFFER_MAX_WIDTH 1 << 10
#define BUFFER_MAX_HEIGHT 1 << 7

typedef struct Context Context;
typedef struct Buffer Buffer;

/**
 * //
 * ////
 * //////    Context class
 * ////////
 * ////////// 
*/

/**
 * The context class provides us with a way to insert ANSI escape codes into our buffer.
 * 
 * @class
*/
struct Context {
  char sContext[32];        // At most 32 characters of an ANSI escape code
  
  Context *pPrevContext;    // A reference to the next context
  Context *pNextContext;    // A reference to the previous context
};

/**
 * Allocates memory for an instance of the Context class.
 * 
 * @return	{ Context * }		A pointer to the allocated memory.
*/
Context *Context_new() {
  Context *pContext = calloc(1, sizeof(*pContext));
  return pContext;
}

/**
 * Initializes an instance of the Context class.
 * 
 * @param		{ Context * }		this	A pointer to the instance to initialize.
 * @return	{ Context * }					A pointer to the initialized instance.
*/
Context *Context_init(Context *this) {

  return this;
}

/**
 * Creates an initialized instance of the Context class.
 * 
 * @return	{ Context * }		A pointer to the newly created initialized instance.
*/
Context *Context_create() {
  return Context_init(Context_new());
}

/**
 * Deallocates the memory of an instance of the Context class.
 * 
 * @param		{ Context * }		this	A pointer to the instance to deallocate.
*/
Context *Context_kill(Context *this) {
  free(this);
}

/**
 * //
 * ////
 * //////    Buffer class
 * ////////
 * ////////// 
*/

/**
 * Buffer class yes
 * @class
*/
struct Buffer {
  int dWidth;                                         // The maximum width of the buffer (in characters).
  int dHeight;                                        // The maximum height of the buffer (in lines).

  char cArray[BUFFER_MAX_HEIGHT][BUFFER_MAX_WIDTH];   // An array that stores the string contents of each of the lines in the buffer.
};

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
  int i, j;

  this->dWidth = dWidth < BUFFER_MAX_WIDTH ? dWidth : BUFFER_MAX_WIDTH;
  this->dHeight = dHeight < BUFFER_MAX_HEIGHT ? dHeight : BUFFER_MAX_HEIGHT;

  // Initialize everything to a space character
  for(i = 0; i < BUFFER_MAX_HEIGHT; i++)
    for(j = 0; j < BUFFER_MAX_WIDTH; j++)
      this->cArray[i][j] = 32;

  return this;
}

/**
 * Creates an initialized instance of the buffer class.
 * Sets its current length to 0.
 * 
 * @param   { Buffer * }  dWidth    The width of each of the buffer lines.
 * @param   { Buffer * }  dHeight   The height of the buffer.
 * @return  { Buffer * }            The pointer to the initialized instance.
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
 * Writes onto a rectangular section of the buffer, starting on (x, y).
 * 
 * @param   { Buffer * }  this    The buffer to modify.
 * @param   { int }       x       The x-coordinate where to start writing in the buffer.
 * @param   { int }       y       The y-coordinate where to start writing in the buffer.
 * @param   { int }       w       The width of the area to write on.
 * @param   { int }       h       The height of the area to write on.
 * @param   { char * }    sBlock  The text content to write onto the area.
*/
void Buffer_write(Buffer *this, int x, int y, int w, int h, char *sBlock[]) {
  int i, j;

  // Copy the characters onto the buffer, as long as it doesn't go beyond the edges
  for(i = y; i < y + h && i < this->dHeight; i++)
    for(j = x; j < x + w && j < this->dWidth; j++)
      this->cArray[i][j] = sBlock[i - y][j - x];
}

/**
 * Outputs the buffer to the screen as one massive blob.
 * 
*/
void Buffer_print(Buffer *this) {
  int i;

  // ! change the size of this later to account for ANSI sequences
  char *sBlob = calloc((this->dWidth + 1) * this->dHeight, sizeof(char));

  // Iterate through the lines
  for(i = 0; i < this->dHeight; i++) {
    strncat(sBlob, this->cArray[i], this->dWidth);
    strcat(sBlob, "\n");
  }

  // Set the buffer size and print using puts()
  IO_setBuffer(sizeof(sBlob));
  puts(sBlob);
}

#endif
