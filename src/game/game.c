/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-28 10:55:29
 * @ Modified time: 2024-03-28 22:57:40
 * @ Modified time: 2024-03-28 23:36:53
 * 
 * Holds the game struct that stores all of the game state.
 */

#ifndef GAME_
#define GAME_

#include "./field.obj.h"

#include "../utils/utils.grid.h"
#include "../utils/utils.string.h"

#define GAME_CELL_HEIGHT 2
#define GAME_CELL_WIDTH 4

#define LEVELS_FILE_PATH "../data/levels.data.txt"
#define LEVELS_FOLDER_PATH ".../build/levels/"

#define LEVEL_NAME_MAX_LENGTH (1 << 4)
#define LEVEL_PATH_MAX_SIZE strlen(LEVELS_FOLDER_PATH) + strlen(LEVELS_FILE_PATH) + sizeof(char)*5

#define GAME_EASY_ROWS     8
#define GAME_EASY_COLUMNS  8
#define GAME_EASY_MINES    10

#define GAME_DIFFICULT_ROWS    10
#define GAME_DIFFICULT_COLUMNS 15 
#define GAME_DIFFICULT_MINES   35

typedef enum GameType GameType;
typedef enum GameDifficulty GameDifficulty;
typedef enum GameOutcome GameOutcome;

typedef struct Game Game;

// Game types chosen by the user
enum GameType {
  GAME_TYPE_CLASSIC,            // Classic game type
  GAME_TYPE_CUSTOM              // Custom game type
};

enum GameDifficulty {
  GAME_DIFFICULTY_NONE,         // It's actually a custom game
  GAME_DIFFICULTY_EASY,         // Classic game: Easy
  GAME_DIFFICULTY_DIFFICULT     // Classic game: Difficult
};

enum GameOutcome {
  GAME_OUTCOME_PENDING,         // The game is still ongoing
  GAME_OUTCOME_QUIT,            // The game ends by the player quitting manually
  GAME_OUTCOME_LOSS,            // The game ends by the player losing
  GAME_OUTCOME_WIN              // The game ends by the player winning
};


struct Game {
  Field gameField;              // Game field
  
  int dCursorX, dCursorY;       // The cursor of the player
  int dTime;                    // Game timer

  GameType eType;
  GameDifficulty eDifficulty;   
  GameOutcome eOutcome;         // Some data about the game
};

/**
 * Initializes the game object.
 * 
 * @param   { Game * }  this  The game object.
*/
void Game_setup(Game *this, GameType eGameType, GameDifficulty eGameDifficulty) {

  // Set the timer and some other params
  this->dTime = 0;
  this->eOutcome = GAME_OUTCOME_PENDING;
  this->eType = eGameType;
  this->eDifficulty = eGameDifficulty;

  // The game's cursor
  this->dCursorX = 0;
  this->dCursorY = 0;
}

/**
 * Sets up the field of the game based on the type and the difficulty.
 * 
 * @param   { Game * }  this  The game object to set up.
*/
void Game_init(Game *this) {

  // Classic mode
  if(this->eType == GAME_TYPE_CLASSIC) {

    // For easy mode
    if(this->eDifficulty == GAME_DIFFICULTY_EASY) {
        
      // Sets up the field's width and height
      Field_init(&this->gameField, GAME_EASY_COLUMNS, GAME_EASY_ROWS);
      
      // Populates the field with mines 
      Field_populateRandom(&this->gameField, GAME_EASY_MINES);
    }
    
    // For difficult mode
    else {

      // Sets up the field's width and height
      Field_init(&this->gameField, GAME_DIFFICULT_COLUMNS, GAME_DIFFICULT_ROWS);

      // Populates the field with mines
      Field_populateRandom(&this->gameField, GAME_DIFFICULT_MINES);
    }
  
  // Custom mode
  } else {
    // ! change this later,, read from file instead
    Field_init(&this->gameField, 10, 10);
  }

  Field_setNumbers(&this->gameField);
}

/**
 * Ends the game.
 * 
 * @param   { Game * }           this         The data of the recently-ended game.
 * @param   { GameOutcome }      eOutcome     How the game was ended.
*/
void Game_end(Game *this, GameOutcome eOutcome) {

  // Saves the outcome to the game data
  this->eOutcome = eOutcome;
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
  for(y = 0; y < dHeight; y++) {
    for(i = 0; i < 2; i++) {
      
      // For each cell in the row
      for(x = 0; x < dWidth; x++) {

        // The number to be shown
        dNumber = this->gameField.aNumbers[y][x];
        dNumber = dNumber < 0 ? 'X' : dNumber + 48;

        // If the cell hasn't been inspected, turn it into a space
        if(!Grid_getBit(this->gameField.pInspectGrid, x, y))
          dNumber = 32;

        // Create the text displaying the number
        sprintf(sNumberText, "| %c ", dNumber);

        // If upper left corner
        if(!y && !x) {
          switch(i) {
            case 0: strcat(sOutputBuffer, "╔───"); break;
            case 1: strcat(sOutputBuffer, sNumberText); break;
          }
        
        // Top edge
        } else if(!y && x) {
          switch(i) {
            case 0: strcat(sOutputBuffer, "╦───"); break;
            case 1: strcat(sOutputBuffer, sNumberText); break;
          }

        // Center pieces
        } else if(y && x) {
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
      if(!y) {
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
  for(x = 0; x < dWidth; x++) {
    for(i = 0; i < 4; i++) {

      // Bottom right corner
      if(x == dWidth - 1 && i == 3) strcat(sOutputBuffer, "─╝");

      // Bottom left corner
      else if(!x && !i) strcat(sOutputBuffer, "╚");
      
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
 * Inspects a tile.
 * 
 * @param   { Field * }     pField      The field to be modified.
 * @param   { int }         x           The tile's x-coordinate in index notation.
 * @param   { int }         y           The tile's y-coordinate in index notation.
*/
void Game_inspect(Game *this, int x, int y) {
  int i, j;

  Field *pField = &this->gameField;

  // If there is a flag there, don't inspect it
  if(Grid_getBit(pField->pFlagGrid, x, y))
    return;

  // Considers the specific tile inspected
  Field_inspect(pField, x, y);

  // Ends the game if a mine has been inspected
  if(Grid_getBit(pField->pMineGrid, x, y)) {
    Game_end(this, GAME_OUTCOME_LOSS);
    return;
  }

  // Cascades the inspection if the number on the tile is 0
  if(pField->aNumbers[y][x] == 0) {

    // The following loops check each adjacent tile within bounds of the field
    // Loops through each row
    for(i = x - 1; i <= x + 1; i++) {
      if(i >= 0 && i <= pField->dWidth - 1) {

        // Loops through each column
        for(j = y - 1; j <= y + 1; j++) {
          if(j >= 0 && j <= pField->dHeight - 1) {

            // Recures the function if the number on the tile is 0
            // only when it hasn't been inspected
            if(!pField->aNumbers[j][i] && 
              !Grid_getBit(pField->pInspectGrid, i, j))
              Game_inspect(this, i, j);

            // Marks the tile as inspected
            if(pField->aNumbers[j][i] >= 0)
              Field_inspect(pField, i, j);

          }
        }
      }
    }
  }
}

/**
 * Adds a flag on a tile only if it hasn't been inspected.
 * Also marks it as inspected.
 * 
 * @param   { Game * }     this     The game object to be modified.
*/
void Game_addFlag (Game *this) {
  if(!Grid_getBit(this->gameField.pInspectGrid, this->dCursorX, this->dCursorY))
    Grid_setBit(this->gameField.pFlagGrid, this->dCursorX, this->dCursorY, 1);
}

/**
 * Removes a flag from a tile only if it currently has a flag.
 * Also marks it as uninspected.
 * 
 * @param   { Game * }      this      The game object to be modified.
*/
void Game_removeFlag (Game *this) {
  if(Grid_getBit(this->gameField.pFlagGrid, this->dCursorX, this->dCursorY))
    Grid_setBit(this->gameField.pFlagGrid, this->dCursorX, this->dCursorY, 0);
}

/**
 * Increments the cursor along the x axis (to the right).
 * Skips tiles that have already been inspected.
 * 
 * @param   { Game * }  this  The game object to modify.
*/
void Game_incrementX(Game *this) {
  int dCursorXPrev = this->dCursorX;
  int dCursorXNew = this->dCursorX;
  int dCursorY = this->dCursorY;
  
  // Look for a tile that hasn't been inspected
  // do {
    dCursorXNew = (dCursorXNew + 1) % this->gameField.dWidth;
  // } while(Grid_getBit(this->gameField.pInspectGrid, dCursorXNew, dCursorY) && dCursorXNew != dCursorXPrev);

  // If found, set the cursor there
  this->dCursorX = dCursorXNew;
}

/**
 * Decrements the cursor along the x axis (to the left).
 * Skips tiles that have already been inspected.
 * 
 * @param   { Game * }  this  The game object to modify.
*/
void Game_decrementX(Game *this) {
  int dCursorXPrev = this->dCursorX;
  int dCursorXNew = this->dCursorX;
  int dCursorY = this->dCursorY;
  
  // Look for a tile that hasn't been inspected
  // do {
    dCursorXNew = (dCursorXNew - 1 + this->gameField.dWidth) % this->gameField.dWidth;
  // } while(Grid_getBit(this->gameField.pInspectGrid, dCursorXNew, dCursorY) && dCursorXNew != dCursorXPrev);

  // If found, set the cursor there
  this->dCursorX = dCursorXNew;
}

/**
 * Increments the cursor along the y axis (down).
 * Skips tiles that have already been inspected.
 * 
 * @param   { Game * }  this  The game object to modify.
*/
void Game_incrementY(Game *this) {
  int dCursorYPrev = this->dCursorY;
  int dCursorYNew = this->dCursorY;
  int dCursorX = this->dCursorX;
  
  // Look for a tile that hasn't been inspected
  // do {
    dCursorYNew = (dCursorYNew + 1) % this->gameField.dHeight;
  // } while(Grid_getBit(this->gameField.pInspectGrid, dCursorX, dCursorYNew) && dCursorYNew != dCursorYPrev);

  // If found, set the cursor there
  this->dCursorY = dCursorYNew;
}

/**
 * Decrements the cursor along the y axis (up).
 * Skips tiles that have already been inspected.
 * 
 * @param   { Game * }  this  The game object to modify.
*/
void Game_decrementY(Game *this) {
  int dCursorYPrev = this->dCursorY;
  int dCursorYNew = this->dCursorY;
  int dCursorX = this->dCursorX;
  
  // Look for a tile that hasn't been inspected
  // do {
    dCursorYNew = (dCursorYNew - 1 + this->gameField.dHeight) % this->gameField.dHeight;
  // } while(Grid_getBit(this->gameField.pInspectGrid, dCursorX, dCursorYNew) && dCursorYNew != dCursorYPrev);

  // If found, set the cursor there
  this->dCursorY = dCursorYNew;
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

