/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-21 11:49:28
 * @ Modified time: 2024-04-01 05:04:30
 * @ Description:
 * 
 * The field stores a grid object and can help us perform operations like 
 *    generating mines, comparing flags, and computing adjacent mines.
 */

#ifndef GAME_FIELD_
#define GAME_FIELD_

#include "../utils/utils.grid.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#define FIELD_MAX_ROWS    10
#define FIELD_MAX_COLUMNS 15

typedef struct Field Field;

/**
 * The field object.
 * This is not a class (because it doesn't need to be instantiated for the game).
*/
struct Field {

  int dWidth;       // The width of the grid
  int dHeight;      // The height of the grid
  int dMines;       // The total number of mines

  Grid *pMineGrid;  // Where we store the mines
  Grid *pFlagGrid;  // Where we store the flags

  // Determines which specific tiles have been inspected
  Grid *pInspectGrid; 

  // Determines the number of mines adjacent to each tile without a mine
  int aNumbers[FIELD_MAX_ROWS][FIELD_MAX_COLUMNS];
};

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
  int i, j;

  this->dWidth = dWidth;
  this->dHeight = dHeight;
  this->dMines = 0;

  this->pMineGrid = Grid_create(dWidth, dHeight);
  this->pFlagGrid = Grid_create(dWidth, dHeight);
  this->pInspectGrid = Grid_create(dWidth, dHeight);

  for(i = 0; i < dHeight; i++)
    for(j = 0; j < dWidth; j++)
      this->aNumbers[i][j] = 0;
}

/**
 * Populates the mine grid with random mines.
 * 
 * @param   { Field * }   this    The field object to modify.
 * @param   { int }       dMines  The number of mines to create
*/
void Field_populateRandom(Field *this, int dMines) {

  // If there's too many mines to fit the grid, we won't allow that
  if(dMines >= this->dWidth * this->dHeight)
    return;

  // Set the current number of mines to 0
  this->dMines = 0;

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

    // Increment number of mines
    this->dMines++;
  }
}

/**
 * Populates the mine grid with mines from a custom level.
 * 
 * @param   { Field * }   this    The field object to modify.
 * @param   { char * }    sPath   The path of the level's file.
*/
void Field_populateCustom(Field *this, char *sPath) {
  int i, j;
  char cTile;
  
  // Opens the file of the custom level
  FILE *pLevel = fopen(sPath, "r");

  // File was not found
  if(pLevel == NULL)
    return; // TODO: error-handling
  
  // Set the number of mines to 0 at first
  this->dMines = 0;

  // Gets the width and height of the field
  fscanf(pLevel, "%d %d ", &this->dWidth, &this->dHeight);

  // Loops through each tile
  for(i = 0; i < this->dWidth; i++) {
    for(j = 0; j < this->dHeight; j++) {

      // Gets the tile's representing character
      fscanf(pLevel, "%c ", &cTile);

      // Places a mine on the tile if its character is 'X'
      if(cTile == 'X') {
        Grid_setBit(this->pMineGrid, i, j, 1);
        this->dMines++;

      // Does not place a mine on the tile if its character is '.'
      } else {
        Grid_setBit(this->pMineGrid, i, j, 0);
      }
    }
  }

  fclose(pLevel);
}

/**
 * Specifies the number of mines adjacent to each tile.
 * 
 * @param   { Field * }   this    The field object to modify.
*/
void Field_setNumbers(Field *this) {
  int i, j, k, l;
  int dMines; // Number of mines adjacent to the tile

  // Loops through each tile
  for(i = 0; i < this->dWidth; i++) {
    for(j = 0; j < this->dHeight; j++) {

      // Sets the initial count to 0
      dMines = 0;

      // Sets dMines to -1 if a mine has been found
      if(Grid_getBit(this->pMineGrid, i, j))
        dMines = -1;

      // Executes the following code if a mine has not been found
      else {

        // The following loops checks each adjacent tile

        // Loops through each row (top, middle, bottom)
        for(k = i - 1; k <= i + 1; k++) {
          if(k >= 0 && k <= this->dWidth - 1) {  // Checks if the tile is within bounds (row)

            // Loops through each column (left, middle, right)
            for(l = j - 1; l <= j + 1; l++) {
                if(j >= 0 && j <= this->dHeight - 1) {  // Checks if the tile is within bounds (col)

                  // Increments dMines if an adjacent mine has been found
                  if(Grid_getBit(this->pMineGrid, k, l))
                    dMines++;
                }
            }
          }
        }

      }

      // Sets the number of adjacent mines
      this->aNumbers[j][i] = dMines;
    }
  }
}

/**
 * Clears the mines on the mine grid.
 * 
 * @param   { Field * }   this  The field object to modify.
*/
void Field_clearMines(Field *this) {
  Grid_clear(this->pMineGrid, 0);
  Field_setNumbers(this);
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
 * Considers a tile inspected.
 * 
 * @param   { Field * }   this    The field object to modify.
 * @param   { int }       x       The x-coordinate of the tile to be inspected.
 * @param   { int }       y       The y-coordinate of the tile to be inspected.
*/
void Field_inspect(Field *pField, int x, int y) {
  Grid_setBit(pField->pInspectGrid, x, y, 1);
}

#endif