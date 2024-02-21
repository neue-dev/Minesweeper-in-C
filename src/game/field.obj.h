/**
 * @ Author: Mo David
 * @ Create Time: 2024-02-21 11:49:28
 * @ Modified time: 2024-02-21 12:05:01
 * @ Description:
 * 
 * The field stores a grid object and can help us perform operations like 
 *    generating mines, comparing flags, and computing adjacent mines.
 */

#ifndef GAME_FIELD
#define GAME_FIELD

#include "../utils/utils.grid.h"

#include <stdlib.h>

/**
 * The field object.
 * This is not a class (because it doesn't need to be instantiated for the game).
*/
typedef struct Field {

  int dWidth;       // The width of the grid
  int dHeight;      // The height of the grid

  Grid *pMineGrid;  // Where we store the mines
  Grid *pFlagGrid;  // Where we store the flags

} Field;

/**
 * Initializes the field object.
 * We give the field object a new instance of the grid class.
 * Note that this time the return type is void since we're not treating this 
 *    construct as a class.
 * 
 * @param   { Field * }   this      The field object to modify.
 * @param   { int }       dWidth    The width of the field.
 * @param   { int }       dHeight   The height of the field.
*/
void Field_init(Field *this, int dWidth, int dHeight) {
  this->dWidth = dWidth;
  this->dHeight = dHeight;

  this->pMineGrid = Grid_create(dWidth, dHeight);
  this->pFlagGrid = Grid_create(dWidth, dHeight);
}

/**
 * Clears the mines on the mine grid.
*/
void Field_clearMines() {
  
}

/**
 * Clears the flags on the flag grid.
*/
void Field_clearFlags() {
  
}

/**
 * Populates the mine grid with random mines.
 * 
 * @param   { int }   dMines  The number of mines to create
*/
void Field_populate() {
  // srand();
}

#endif