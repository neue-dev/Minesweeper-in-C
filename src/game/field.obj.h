/**
 * @ Author: Mo David
 * @ Create Time: 2024-02-21 11:49:28
 * @ Modified time: 2024-02-22 06:58:11
 * @ Description:
 * 
 * The field stores a grid object and can help us perform operations like 
 *    generating mines, comparing flags, and computing adjacent mines.
 */

#ifndef GAME_FIELD
#define GAME_FIELD

#include "../utils/utils.grid.h"

#include <stdlib.h>
#include <time.h>

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
 * 
 * @param   { Field * }   this  The field object to modify.
*/
void Field_clearMines(Field *this) {
  Grid_clear(this->pMineGrid, 0);
}

/**
 * Clears the flags on the flag grid.
 * 
 * @param   { Field * }   this  The field object to modify.
*/
void Field_clearFlags(Field *this) {
  Grid_clear(this->pFlagGrid, 0);
}

/**
 * Populates the mine grid with random mines.
 * 
 * @param   { Field * }   this    The field object to modify.
 * @param   { int }       dMines  The number of mines to create
*/
void Field_populate(Field *this, int dMines) {

  // If there's too many mines to fit the grid, we won't allow that
  if(dMines >= this->dWidth * this->dHeight)
    return;

  // We will use a clock to seed our RNG
  srand((unsigned) time(NULL));

  // Produce the given number of mines
  while(dMines--) {

    // Choose a location to plant a mine
    int dLocation = rand() % (this->dWidth * this->dHeight);

    // While the selected bit already has a mine
    while(Grid_getBit(this->pMineGrid, 
      dLocation % this->dWidth,       // The x-value of the bit 
      dLocation / this->dWidth)) {    // The y-value of the bit
      
      // Move to another cell and loop start if end is reached
      dLocation++;
      dLocation %= (this->dWidth * this->dHeight);
    }

    // Set the chosen bit
    Grid_setBit(this->pMineGrid, 
      dLocation % this->dWidth, 
      dLocation / this->dWidth, 1);
  }
}

#endif