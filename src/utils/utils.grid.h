/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-21 10:53:17
 * @ Modified time: 2024-03-02 16:25:54
 * @ Description:
 * 
 * A grid class that can help us create blocks of text before printing them.
 */

#ifndef UTILS_GRID_
#define UTILS_GRID_

#include "../utils/utils.types.h"

#include <stdlib.h>

#define GRID_MAX_SIZE 64

typedef struct Grid Grid;

/**
 * //
 * ////
 * //////    Grid class
 * ////////
 * ////////// 
*/

/**
 * Grid class yes
 * @class
*/
struct Grid {
  int dWidth;                         // The width of the grid (in bits).
  int dHeight;                        // The height of the grid (in lines).

  uint64_t dBitArray[GRID_MAX_SIZE];  // A 32 x 32 grid of bits
                                      // This means that our grid can have a maximum size of 32 x 32,
                                      //    although the project specifications call for much less.
};

/**
 * Allocates memory for a new instance of the grid class.
 * 
 * @return  { Grid * }  A pointer to the location in memory of the grid instance.
*/
Grid *Grid_new() {
  Grid *pGrid = calloc(1, sizeof(*pGrid));
  return pGrid;
}

/**
 * Initializes an instance of the grid class.
 * If the function receives a width or height that is too large, 32 becomes
 *    the default initialization of that parameter.
 * 
 * @param   { Grid * }  this      A pointer to the instance we're going to init.
 * @param   { Grid * }  dWidth    The width of the bit array we wish to use.
 * @param   { Grid * }  dHeight   The height of the bit array we wish to use.
 * @return  { Grid * }            The pointer to the initialized instance.
*/
Grid *Grid_init(Grid *this, int dWidth, int dHeight) {
  int i;

  // Set all the bit strings to 0
  for(i = 0; i < GRID_MAX_SIZE; i++)
    this->dBitArray[i] = 0;

  // If the dimensions are too big, use default
  this->dWidth = dWidth < GRID_MAX_SIZE ? dWidth : GRID_MAX_SIZE;
  this->dHeight = dHeight < GRID_MAX_SIZE ? dHeight : GRID_MAX_SIZE;

  return this;
}

/**
 * Creates an initialized instance of the grid class.
 * Sets its current length to 0.
 * 
 * @param   { Grid * }  dWidth    The width of each of the grid lines.
 * @param   { Grid * }  dHeight   The height of the grid.
 * @return  { Grid * }            The pointer to the initialized instance.
*/
Grid *Grid_create(int dWidth, int dHeight) {
  return Grid_init(Grid_new(), dWidth, dHeight);
}

/**
 * Frees the memory associated with an instance of the grid class.
 * 
 * @param   { Grid * }  this  The instance to be freed from memory.
*/
void Grid_kill(Grid *this) {
  free(this);
}


/**
 * Returns the value at an entry on the grid.
 * This function assumes that x and y are within range of the dimensions of the grid.
 * 
 * @param   { Grid * }  this  The grid instance we wish to read.
 * @param   { int }     x     The x-coordinate of the bit (which column its at).
 * @param   { int }     y     The y-coordinate of the bit (which row its at).
 * @return  { int }           Whether or not the bit is a 1 or 0.
*/
int Grid_getBit(Grid *this, int x, int y) {
  uint64_t dMask = 1ULL << (GRID_MAX_SIZE - 1);

  return (this->dBitArray[y] & dMask >> x) ? 1 : 0;
}

/**
 * Sets the value of a certain bit on the grid. 
 * 
 * @param   { Grid * }  this  The grid instance we wish to modify.
 * @param   { int }     x     The x-coordinate of the bit (which column its at).
 * @param   { int }     y     The y-coordinate of the bit (which row its at).
 * @param   { int }     n     The new value we want to set the bit to.
*/
void Grid_setBit(Grid *this, int x, int y, int n) {
  uint64_t dMask = 1ULL << (GRID_MAX_SIZE - 1);

  if(n) this->dBitArray[y] |= dMask >> x;     // If we're making the bit truthy, we need an |=
  else this->dBitArray[y] &= ~(dMask >> x);   // Otherwise, an &= with the inverse of the mask works
}

/**
 * Clears the values stored by a grid instance (sets them all to 0 or 1).
 * 
 * @param   { Grid * }  this  The grid instance we wish to modify.
 * @param   { int }     n     Whether or not to set all bits to 1 or 0.
*/
void Grid_clear(Grid *this, int n) {
  int i;

  // Set them all back to 0
  for(i = 0; i < this->dHeight; i++)
    this->dBitArray[i] = n ? -1 : 0;
}

#endif
