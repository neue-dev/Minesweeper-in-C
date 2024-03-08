/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-20 02:22:07
 * @ Modified time: 2024-03-08 09:44:05
 * @ Description:
 *   
 * A buffer class that can help us create blocks of text before printing them.
 */

#ifndef UTILS_BUFFER_
#define UTILS_BUFFER_

#include "./utils.io.h"
#include "./utils.string.h"
#include "./utils.graphics.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define BUFFER_MAX_WIDTH (1 << 8)
#define BUFFER_MAX_HEIGHT (1 << 6)
#define BUFFER_MAX_CONTEXTS (1 << 10)

typedef struct Buffer Buffer;

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
  int dWidth;                                                         // The maximum width of the buffer (in characters).
  int dHeight;                                                        // The maximum height of the buffer (in lines).

  int dDefaultFG;
  int dDefaultBG;

  char cContentArray[BUFFER_MAX_HEIGHT][BUFFER_MAX_WIDTH][4];         // An array that stores the string contents of each of the lines in the buffer.
                                                                      // The reason we multiply the width by 4 is because we have to account for the fact that
                                                                      //    some of the characters we will be using might need more than 8 bytes to be
                                                                      //    represented (in other words, more than one char unit).

  int dContextCount;                                                  // How many contexts we currently have
  char *sDefaultContext;                                              // The default ANSI escape sequence for the FG and BG color of the buffer
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
 * @param   { int }       dDefaultFG  The default foreground color of the buffer.
 * @param   { int }       dDefaultBG  The default background color of the buffer.
 * @return  { Buffer * }              The pointer to the initialized instance.
*/
Buffer *Buffer_init(Buffer *this, int dWidth, int dHeight, int dDefaultFG, int dDefaultBG) {
  int i, j;

  this->dWidth = dWidth < BUFFER_MAX_WIDTH ? dWidth : BUFFER_MAX_WIDTH;       // The number of characters along each row
  this->dHeight = dHeight < BUFFER_MAX_HEIGHT ? dHeight : BUFFER_MAX_HEIGHT;  // The number of rows for the buffer

  // Set the default config of the styling
  this->dDefaultFG = dDefaultFG;
  this->dDefaultBG = dDefaultBG;
  this->sDefaultContext = Graphics_getCodeFGBG(dDefaultFG, dDefaultBG);

  // No contexts at the moment
  this->dContextCount = 0;

  // Initialize everything to a space character
  // Also, initialize the context mask values to 0
  for(i = 0; i < BUFFER_MAX_HEIGHT; i++) {
    for(j = 0; j < BUFFER_MAX_WIDTH; j++) {
      this->cContentArray[i][j][0] = 32;
    }
  }
  
  // Initialize the contest mask and offset arrays to 0
  for(i = 0; i < BUFFER_MAX_HEIGHT; i++) {
    for(j = 0; j < BUFFER_MAX_WIDTH; j++) {
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
 * @param   { int }       dDefaultFG  The default foreground color of the buffer.
 * @param   { int }       dDefaultBG  The default background color of the buffer.
 * @return  { Buffer * }              The pointer to the initialized instance.
*/
Buffer *Buffer_create(int dWidth, int dHeight, int dDefaultFG, int dDefaultBG) {
  return Buffer_init(Buffer_new(), dWidth, dHeight, dDefaultFG, dDefaultBG);
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
void Buffer_write(Buffer *this, int x, int y, int h, char *sBlock[]) {
  int i, j, j2, k, w;  // j2 is the second iterator we will use in sBlock

  // Copy the characters onto the buffer, as long as it doesn't go beyond the edges
  for(i = y; i < y + h && i < this->dHeight; i++) {
    w = strlen(sBlock[i - y]);

    // In case i is negative, since we're allowing that
    if(i >= 0) {

      // Go through each character
      for(j = x, j2 = 0; j < this->dWidth && j2 < w; j++) {

        // We will permit negative passing values for cooler animations
        // So this check becomes necessary
        if(j >= 0) {
          
          // This way, we can have text blocks that arent strictly rectangular
          if(sBlock[i - y][j2] != 32) {
            
            // Copy the char
            if(String_isChar(sBlock[i - y][j2])) {
              this->cContentArray[i][j][0] = sBlock[i - y][j2++];

            // The char occupies more than 1 byte
            } else {
              k = 0;
              
              // While we're within a compound character
              do {
                this->cContentArray[i][j][k++] = sBlock[i - y][j2++];
                
              } while(
                !String_isChar(sBlock[i - y][j2]) &&
                !String_isStartChar(sBlock[i - y][j2]));

              while(k < 4)
                this->cContentArray[i][j][k++] = 0;
            }

          // It's just a whitespace
          } else {
            j2++;
          }
        }
      }
    }
  }
}

/**
 * Creates a context within the buffer.
 * In this case, a context is a basically a rectangular slice of the 2d array where a certain style
 *    (be it a color change or something else) is applied to that slice.
 * 
 * @param   { Buffer * }  this      The buffer to modify.
 * @param   { int }       x         The x-coordinate where the context begins in the buffer.
 * @param   { int }       y         The y-coordinate where the context begins in the buffer.
 * @param   { int }       w         The width of the context area.
 * @param   { int }       h         The height of the context area.
 * @param   { color }     colorFG   The color of the foreground within the context.
 * @param   { color }     colorBG   The color of the background within the context.
*/
void Buffer_contextRect(Buffer *this, int x, int y, int w, int h, color colorFG, color colorBG) {
  int i, j;

  // Can't overload ourselves
  if(this->dContextCount >= BUFFER_MAX_CONTEXTS)
    return;

  // The user didn't specify anything to define the context
  if(colorFG < 0 && colorBG < 0)
    return;

  // Append the new context
  if(colorFG < 0)
    this->sContextArray[this->dContextCount++] = Graphics_getCodeBG(colorBG);
  else if(colorBG < 0)
    this->sContextArray[this->dContextCount++] = Graphics_getCodeFG(colorFG);
  else
    this->sContextArray[this->dContextCount++] = Graphics_getCodeFGBG(colorFG, colorBG);

  // Set the appropriate context values to the index + 1 of the created context
  for(i = y; i < y + h && i < this->dHeight; i++) 
    for(j = x; j < x + w && j < this->dWidth; j++) 
      if(i >= 0 && j >= 0)
        this->dContextMask[i][j] = this->dContextCount;

}

/**
 * Creates a circular context within the buffer.
 * A context here is a basically a circular slice of the 2d array where a certain style
 *    (be it a color change or something else) is applied to that slice.
 * 
 * @param   { Buffer * }  this      The buffer to modify.
 * @param   { int }       x         The x-coordinate where the context is centered in the buffer.
 * @param   { int }       y         The y-coordinate where the context is centered in the buffer.
 * @param   { int }       w         The radius of the context area.
 * @param   { color }     colorFG   The color of the foreground within the context.
 * @param   { color }     colorBG   The color of the background within the context.
*/
void Buffer_contextCircle(Buffer *this, int x, int y, int r, color colorFG, color colorBG) {
  int i, j;

  // Minimum radius would be 1
  if(r < 1)
    return;

  // Can't overload ourselves
  // The plus 2 is cuz we're adding three new contexts
  if(this->dContextCount + 2 >= BUFFER_MAX_CONTEXTS)
    return;

  // The user didn't specify anything to define the context
  if(colorFG < 0 && colorBG < 0)
    return;

  // Append the new contexts
  if(colorFG < 0) {
    this->sContextArray[this->dContextCount++] = Graphics_getCodeBG(colorBG);
    this->sContextArray[this->dContextCount++] = Graphics_getCodeBG(Graphics_lerp(this->dDefaultBG, colorBG, 0.8));
    this->sContextArray[this->dContextCount++] = Graphics_getCodeBG(Graphics_lerp(this->dDefaultBG, colorBG, 0.5));

  } else if(colorBG < 0) {
    this->sContextArray[this->dContextCount++] = Graphics_getCodeFG(colorFG);
    this->sContextArray[this->dContextCount++] = Graphics_getCodeFG(Graphics_lerp(this->dDefaultFG, colorFG, 0.8));
    this->sContextArray[this->dContextCount++] = Graphics_getCodeFG(Graphics_lerp(this->dDefaultFG, colorFG, 0.5));

  } else {
    this->sContextArray[this->dContextCount++] = Graphics_getCodeFGBG(colorFG, colorBG);
    this->sContextArray[this->dContextCount++] = Graphics_getCodeFGBG(
      Graphics_lerp(this->dDefaultFG, colorBG, 0.8),
      Graphics_lerp(this->dDefaultBG, colorBG, 0.8));
    this->sContextArray[this->dContextCount++] = Graphics_getCodeFGBG(
      Graphics_lerp(this->dDefaultFG, colorBG, 0.5),
      Graphics_lerp(this->dDefaultBG, colorBG, 0.5));
      
  }

  // Set the appropriate context values to the index + 1 of the created context
  // Note that we do r * 2 along the x because the height of a character is twice its width
  for(i = y - r; i < y + r && i < this->dHeight; i++) 
    for(j = x - r * 2; j < x + r * 2 && j < this->dWidth; j++) 
      if(i >= 0 && j >= 0) {
        
        // I find it weird how / 5.0 produces better results than / 4.0, even though
        //    mathematically / 4.0 makes more sense in this context.
        if(
          round(
            (i - y + 0.5) * (i - y + 0.5) + 
            (j - x + 0.5) * (j - x + 0.5) / 5.0
          ) 
          <= (r - 1) * (r - 1))
          
          this->dContextMask[i][j] = this->dContextCount - 2;
        
        // Don't mind the unorthodox formatting; this is just to see the form of the expressions
        //    much more clearly. Also, these are just here to shade the circles differently.
        else if(
          round(
            (i - y + 0.5) * (i - y + 0.5) + 
            (j - x + 0.5) * (j - x + 0.5) / 4.0
          ) 
          < r * r)
          
          this->dContextMask[i][j] = this->dContextCount - 1;
        
        // The outermost edges of the circle bleed off into the background.
        else if(
          round(
            (i - y + 0.5) * (i - y + 0.5) + 
            (j - x + 0.5) * (j - x + 0.5) / 5.0
          ) 
          <= r * r)
          
          this->dContextMask[i][j] = this->dContextCount;
      }

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
  char *sBlob = String_alloc(((this->dWidth) + 1) * this->dHeight << 3);

  // Iterate through the lines
  for(y = 0; y < this->dHeight; y++) {

    // Set the defaults at the start of every line
    i = 0;
    while(this->sDefaultContext[i]) {
      sBlob[dLen] = this->sDefaultContext[i];
      dLen++; i++;   
    }

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
          while(this->sDefaultContext[i] != 0) {
            sBlob[dLen] = this->sDefaultContext[i];
            dLen++; i++;   
          }
        }
      }
      
      if(String_isChar(this->cContentArray[y][x][0])) {
        sBlob[dLen++] = this->cContentArray[y][x][0];

      } else {
        i = 0;

        while(i < 4 && this->cContentArray[y][x][i])
          sBlob[dLen++] = this->cContentArray[y][x][i++];
      }
    }

    // The condition here fixed a massive problem WTF im so stupid
    if(y + 1 < this->dHeight)
      sBlob[dLen++] = '\n';
  }

  // Set the buffer size and print using puts(), then do garbage collection
  IO_setBuffer(sizeof(sBlob));

  // Windows and Unix have different preferences over which output functions are faster
  String_print(sBlob);
  
  // Clean up all the stuff we used
  for(i = 0; i < this->dContextCount; i++)
    Graphics_delCode(this->sContextArray[i]);

  String_kill(sBlob);
}

#endif
