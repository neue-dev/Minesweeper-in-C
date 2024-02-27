/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-20 02:22:07
 * @ Modified time: 2024-02-27 08:50:10
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

#define BUFFER_MAX_WIDTH 1 << 8
#define BUFFER_MAX_HEIGHT 1 << 6
#define BUFFER_MAX_CONTEXTS 1 << 10

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
typedef struct Buffer Buffer;

struct Buffer {
  int dWidth;                                                         // The maximum width of the buffer (in characters).
  int dHeight;                                                        // The maximum height of the buffer (in lines).

  char *sDefaultFG;                                                   // The default foreground color of the buffer
  char *sDefaultBG;                                                   // The default background color of the buffer

  char cContentArray[BUFFER_MAX_HEIGHT][BUFFER_MAX_WIDTH];            // An array that stores the string contents of each of the lines in the buffer.

  int dContextLength;                                                 // How many contexts we currently have
  char *sContextArray[BUFFER_MAX_CONTEXTS];                           // Stores the different contexts we will be using  
  unsigned short dContextMask[BUFFER_MAX_HEIGHT][BUFFER_MAX_WIDTH];   // Stores the contexts that define the styling of the entire buffer
                                                                      // Note that this is unsigned so we don't waste space; we need the extra memory LMOO
                                                                      // Because of this, 0 has to mean smth like NULL and 1 -> 0, 2 -> 1, etc.
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
 * @param   { Buffer * }  this        A pointer to the instance we're going to init.
 * @param   { int }       dWidth      The width of each of the buffer lines.
 * @param   { int }       dHeight     The height of the buffer.
 * @param   { char * }    sDefaultFG  The default foreground color of the buffer.
 * @param   { char * }    sDefaultBG  The default background color of the buffer.
 * @return  { Buffer * }              The pointer to the initialized instance.
*/
Buffer *Buffer_init(Buffer *this, int dWidth, int dHeight, char *sDefaultFG, char *sDefaultBG) {
  int i, j;

  this->dWidth = dWidth < BUFFER_MAX_WIDTH ? dWidth : BUFFER_MAX_WIDTH;
  this->dHeight = dHeight < BUFFER_MAX_HEIGHT ? dHeight : BUFFER_MAX_HEIGHT;

  this->sDefaultFG = sDefaultFG;
  this->sDefaultBG = sDefaultBG;

  // No contexts at the moment
  this->dContextLength = 0;

  // Initialize everything to a space character
  // Also, initialize the context mask values to 0
  for(i = 0; i < BUFFER_MAX_HEIGHT; i++) {
    for(j = 0; j < BUFFER_MAX_WIDTH; j++) {
      this->cContentArray[i][j] = 32;
      this->dContextMask[i][j] = 0;
    }
  }

  return this;
}

/**
 * Creates an initialized instance of the buffer class.
 * Sets its current length to 0.
 * 
 * @param   { int }       dWidth      The width of each of the buffer lines.
 * @param   { int }       dHeight     The height of the buffer.
 * @param   { char * }    sDefaultFG  The default foreground color of the buffer.
 * @param   { char * }    sDefaultBG  The default background color of the buffer.
 * @return  { Buffer * }            The pointer to the initialized instance.
*/
Buffer *Buffer_create(int dWidth, int dHeight, char *sDefaultFG, char *sDefaultBG) {
  return Buffer_init(Buffer_new(), dWidth, dHeight, sDefaultFG, sDefaultBG);
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
      this->cContentArray[i][j] = sBlock[i - y][j - x];
}

/**
 * Creates a context within the buffer.
 * A context is a basically a rectangular slice of the 2d array where a certain style
 *    (be it a color change or something else) is applied to that slice.
 * 
 * Note that the way this function is implemented means that it will only work for non-overlapping
 *    recatngles (contexts). When they overlap, things MAY break.
 * 
 * @param   { Buffer * }  this      The buffer to modify.
 * @param   { int }       x         The x-coordinate where the context begins in the buffer.
 * @param   { int }       y         The y-coordinate where the context begins in the buffer.
 * @param   { int }       w         The width of the context area.
 * @param   { int }       h         The height of the context area.
 * @param   { char * }    sContext  The style to apply to the area, usually an ANSI escape sequence.
*/
void Buffer_context(Buffer *this, int x, int y, int w, int h, char *sContext) {
  int i, j;

  // Can't overload ourselves
  if(this->dContextLength >= BUFFER_MAX_CONTEXTS)
    return;

  // Append the new context
  this->sContextArray[this->dContextLength] = sContext;

  // Increment the context length
  this->dContextLength++;

  // Set the appropriate context values to the index + 1 of the created context
  for(i = y; i < y + h && i < this->dHeight; i++) 
    for(j = x; j < x + w && j < this->dWidth; j++) 
      this->dContextMask[i][j] = this->dContextLength;

}
      
/**
 * Outputs the buffer to the screen as one massive blob.
 * 
 * @param   { Buffer * }  this  The buffer to modify.
*/
void Buffer_print(Buffer *this) {
  int x, y, i;

  // Some stuff to use for context finding
  int dLastMask;
  int bShouldUpdateContext;

  // The current length of the blob
  // and the blob itself
  int dLen = 0;   
  char *sBlob = calloc((this->dWidth + 1) * this->dHeight * 4, sizeof(char));

  // Set the defaults first
  i = 0;
  while(this->sDefaultFG[i]) {
    sBlob[dLen] = this->sDefaultFG[i];
    dLen++; i++;   
  }

  i = 0;
  while(this->sDefaultBG[i]) {
    sBlob[dLen] = this->sDefaultBG[i];
    dLen++; i++;   
  }

  // Iterate through the lines
  for(y = 0; y < this->dHeight; y++) {

    // The last mask value we had
    dLastMask = 0;

    // Loop through each character in the row
    // Check context changes too
    for(x = 0; x < this->dWidth; x++) {

      // Whether or not to update into a new context
      bShouldUpdateContext = 0;

      // Check through the context mask
      if(dLastMask != this->dContextMask[y][x]) {
        
        // Update the last mask we had
        dLastMask = this->dContextMask[y][x];

        // Something happened, we need to update
        bShouldUpdateContext = 1;
      }

      // Add the context string to the blob
      if(bShouldUpdateContext) {
        i = 0;
        
        // Just copy the new context string char by char
        if(dLastMask) {
          while(this->sContextArray[dLastMask - 1][i]) {
            sBlob[dLen] = this->sContextArray[dLastMask - 1][i];
            dLen++; i++;   
          }

        // If we're going back to "normal", we'll just put back the defaults
        } else {
          while(this->sDefaultFG[i] != 0) {
            sBlob[dLen] = this->sDefaultFG[i];
            dLen++; i++;   
          }

          i = 0;
          while(this->sDefaultBG[i] != 0) {
            sBlob[dLen] = this->sDefaultBG[i];
            dLen++; i++;   
          }
        }
      }
      
      sBlob[dLen] = this->cContentArray[y][x];
      dLen++;
    }

    sBlob[dLen] = '\n';
    dLen++;
  }

  // Set the buffer size and print using puts(), then do garbage collection
  IO_setBuffer(sizeof(sBlob));
  puts(sBlob);
  
  // Clean up all the stuff we used
  for(i = 0; i < this->dContextLength; i++)
    Graphics_delCode(this->sContextArray[i]);
    
  free(sBlob);
}

#endif
