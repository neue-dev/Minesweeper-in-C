/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-28 10:55:29
 * @ Modified time: 2024-03-28 19:29:42
 * @ Description:
 * 
 * Holds the game struct that stores all of the game state.
 */

#ifndef GAME_
#define GAME_

#include "./field.obj.h"
#include "./gameplay.game.c"
#include "../utils/utils.grid.h"

#define GAME_CELL_HEIGHT 2
#define GAME_CELL_WIDTH 4

typedef struct Game Game;

struct Game {
  Field gameField;  // Game field
  int dTime;        // Game timer
};

/**
 * Initializes the game object.
 * 
 * @param   { Game * }  this  The game object.
*/
void Game_init(Game *this) {
  this->dTime = 0;

  // ! change dimensions, change numbe rof mines
  Field_init(&this->gameField, 10, 10);
  Field_populateRandom(&this->gameField, 8);
  Field_setNumbers(&this->gameField);
}

/**
 * Creates a display of the grid as a text asset.
 * 
 * @param   { Game * }   this   The field object to display.
 * @return  { char * }          The text asset representing the grid.
*/
void Game_displayGrid(Game *this, char *sOutputBuffer) {
  int x, y, i, dNumber;

  // The actual field
  // Don't mind why this is a pointer, it didn't have to be LMAO
  Field *pField = &this->gameField;

  // Some important consts
  int dWidth = pField->dWidth;
  int dHeight = pField->dHeight;
  char *sNumberText = calloc(4, 1);

  // For each row
  for(x = 0; x < dHeight; x++) {
    for(i = 0; i < 2; i++) {
      
      // For each cell in the row
      for(y = 0; y < dWidth; y++) {

        // The number to be shown
        dNumber = this->gameField.aNumbers[x][y];
        dNumber = dNumber < 0 ? 'X' : dNumber + 48;

        // If the cell hasn't been inspected, turn it into a space
        if(!Grid_getBit(this->gameField.pInspectGrid, x, y))
          dNumber = 32;

        // Create the text displaying the number
        sprintf(sNumberText, "| %c ", dNumber);

        // If upper left corner
        if(!x && !y) {
          switch(i) {
            case 0: strcat(sOutputBuffer, "╔───"); break;
            case 1: strcat(sOutputBuffer, sNumberText); break;
          }
        
        // Top edge
        } else if(!x && y) {
          switch(i) {
            case 0: strcat(sOutputBuffer, "╦───"); break;
            case 1: strcat(sOutputBuffer, sNumberText); break;
          }

        // Center pieces
        } else if(x && y) {
          switch(i) {
            case 0: strcat(sOutputBuffer, "╬───"); break;
            case 1: strcat(sOutputBuffer, sNumberText); break;
          }
          
        // Left edge
        } else {
          switch(i) {
            case 0: strcat(sOutputBuffer, "╠───"); break;
            case 1: strcat(sOutputBuffer, sNumberText); break;
          }
        }
      } 

      // Upper right corner
      if(!x) {
        switch(i) {
          case 0: strcat(sOutputBuffer, "╗"); break;
          case 1: strcat(sOutputBuffer, "│"); break;
        }

      // Right edge
      } else {
        switch(i) {
          case 0: strcat(sOutputBuffer, "╣"); break;
          case 1: strcat(sOutputBuffer, "│"); break;
        }
      }

      strcat(sOutputBuffer, "\n");
    }
  }

  // Bottom edge
  for(y = 0; y < dWidth; y++) {
    for(i = 0; i < 4; i++) {

      // Bottom right corner
      if(y == dWidth - 1 && i == 3) strcat(sOutputBuffer, "─╝");

      // Bottom left corner
      else if(!y && !i) strcat(sOutputBuffer, "╚");
      
      // Bottom edge
      else if(!i) strcat(sOutputBuffer, "╩");
      
      // Everything else
      else strcat(sOutputBuffer, "─");
    }
  }
  
  // Last newline
  strcat(sOutputBuffer, "\n");
  
  // Free the text
  free(sNumberText);
}

/**
 * How many characters wide the grid is.
 * 
 * @param   { Game * }   this   The field object to read.
 * @return  { int }             The width of the field in characters.
*/
int Game_getCharWidth(Game *this) {
  return this->gameField.dWidth * GAME_CELL_WIDTH + 1;
}

/**
 * How many characters tall the grid is.
 * 
 * @param   { Game * }   this   The field object to read.
 * @return  { int }             The height of the field in characters.
*/
int Game_getCharHeight(Game *this) {
  return this->gameField.dHeight * GAME_CELL_HEIGHT + 1;
}

#endif

