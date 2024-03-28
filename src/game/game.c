/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-28 10:55:29
 * @ Modified time: 2024-03-28 16:47:51
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

  // ! change dimensions
  Field_init(&this->gameField, 15, 10);
}

/**
 * Creates a display of the grid as a text asset.
 * 
 * @param   { Game * }   this   The field object to display.
 * @return  { char * }          The text asset representing the grid.
*/
char *Game_displayGrid(Game *this) {
  int x, y, i, dNumber;

  // The actual field
  // Don't mind why this is a pointer, it didn't have to be LMAO
  Field *pField = &this->gameField;

  // Some important consts
  int dWidth = pField->dWidth;
  int dHeight = pField->dHeight;
  char *sGridText = calloc(dWidth * 4 * dHeight * 16, sizeof(char));

  // For each row
  for(y = 0; y < dHeight; y++) {
    for(i = 0; i < 2; i++) {
      
      // For each cell in the row
      for(x = 0; x < dWidth; x++) {

        // The number to be shown
        dNumber = this->gameField.aNumbers[y][x];
        
        // If upper left corner
        if(!y && !x) {
          switch(i) {
            case 0: strcat(sGridText, "╔───"); break;
            case 1: strcat(sGridText, "│   "); break;
          }
        
        // Top edge
        } else if(!y && x) {
          switch(i) {
            case 0: strcat(sGridText, "╦───"); break;
            case 1: strcat(sGridText, "│   "); break;
          }

        // Center pieces
        } else if(y && x) {
          switch(i) {
            case 0: strcat(sGridText, "╬───"); break;
            case 1: strcat(sGridText, "│   "); break;
          }
          
        // Left edge
        } else {
          switch(i) {
            case 0: strcat(sGridText, "╠───"); break;
            case 1: strcat(sGridText, "│   "); break;
          }
        }
      } 

      // Upper right corner
      if(!y) {
        switch(i) {
          case 0: strcat(sGridText, "╗"); break;
          case 1: strcat(sGridText, "│"); break;
        }

      // Right edge
      } else {
        switch(i) {
          case 0: strcat(sGridText, "╣"); break;
          case 1: strcat(sGridText, "│"); break;
        }
      }

      strcat(sGridText, "\n");
    }
  }

  // Bottom edge
  for(x = 0; x < dWidth; x++) {
    for(i = 0; i < 4; i++) {

      // Bottom right corner
      if(x == dWidth - 1 && i == 3) strcat(sGridText, "─╝");

      // Bottom left corner
      else if(!x && !i) strcat(sGridText, "╚");
      
      // Bottom edge
      else if(!i) strcat(sGridText, "╩");
      
      // Everything else
      else strcat(sGridText, "─");
    }
  }
  
  strcat(sGridText, "\n");
  
  return sGridText;
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

