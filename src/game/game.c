/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-28 10:55:29
 * @ Modified time: 2024-03-28 21:40:22
 * @ Modified time: 2024-03-28 21:40:22
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
  
  int dWidth, dHeight;          // Game field dimensions
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
void Game_init(Game *this, GameDifficulty eGameDifficulty) {

  // Set the timer and some other params
  this->dTime = 0;
  this->eOutcome = GAME_OUTCOME_PENDING;
  this->eDifficulty = eGameDifficulty;

  // ! change dimensions, change numbe rof mines
  Field_init(&this->gameField, 10, 10);
  Field_populateRandom(&this->gameField, 8);
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
        } else if(x && !y) {
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
 * Intializes the field's data according to the difficulty.
 * This function is used for classic games.
 * 
 * @param   { GameDifficulty }    eGameDifficulty   The game's difficulty.
 * @param   { Field * }     pField   The field to be modified.
*/
void Gameplay_initClassic(GameDifficulty eDifficulty, Field *pField) {

  // For easy mode
  if(eDifficulty == GAME_DIFFICULTY_EASY) {
      
    // Sets up the field's width and height
    pField->dWidth = GAME_EASY_ROWS;
    pField->dHeight = GAME_EASY_COLUMNS;
    
    // Populates the field with mines 
    Field_populateRandom(pField, GAME_EASY_MINES);
  }
  
  // For difficult mode
  else {

    // Sets up the field's width and height
    pField->dWidth = GAME_DIFFICULT_ROWS;
    pField->dHeight = GAME_DIFFICULT_COLUMNS;

    // Populates the field with mines
    Field_populateRandom(pField, GAME_DIFFICULT_MINES);
  }
}

/**
 * Intializes the field's data according to the custom game's specs.
 * // ! fix
 * 
 * @param   { Field * }     pField   The field to be modified.
 * @param   { sName * }     sName    Name of the custom level.
*/
void Gameplay_initCustom(Field *pField, char *sName) {
    
  // Declares the path of the custom level's file.
  char *sPath = String_alloc(LEVEL_PATH_MAX_SIZE);

  // Sets the cutom level's file path
  snprintf(sPath, LEVEL_PATH_MAX_SIZE, "%s%s.txt", LEVELS_FOLDER_PATH, sName);

  // Populates the field with mines
  Field_populateCustom(pField, sPath);

  // Deallocates the memory for the file path's string
  String_kill(sPath);
}

/**
 * Selects the type of game (classic or custom) to be played.
 * 
 * @param   { GameType }    eType    The type of game to be played.
 * @param   { Field * }     pField   The field to be modified.
*/
void Gameplay_selectType(GameType eType, Field *pField) {

  // For classic games
  if(eType == GAME_TYPE_CLASSIC) {

    GameDifficulty eDifficulty;
    // TODO: input game difficulty

    //! remove this line later on
    eDifficulty = GAME_DIFFICULTY_EASY;

    Gameplay_initClassic(eDifficulty, pField);

  // For custom games
  } else {

    // TODO: input custom level name
    char *sName = "";
    
    // TODO: check if the level exists; if not, handle the error

    Gameplay_initCustom(pField, sName);
  }

  // Initializes each of the tile's flag placement status
  Grid_clear(pField->pFlagGrid, 0);

  // Initializes each of the tile's inspection status
  Grid_clear(pField->pInspectGrid, 0);

  // Specifies the number of mines adjacent to each tile
  Field_setNumbers(pField);
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
  if(pField->aNumbers[x][y] == 0) {

    // The following loops check each adjacent tile within bounds of the field
    // Loops through each row
    for(i = x - 1; i <= x + 1; i++) {
      if(i >= 0 && i <= pField->dHeight - 1) {

        // Loops through each column
        for(j = y - 1; j <= y + 1; j++) {
          if(j >= 0 && j <= pField->dWidth - 1) {

            // Recures the function if the number on the tile is 0
            // only when it hasn't been inspected
            if(!pField->aNumbers[i][j] && 
              !Grid_getBit(pField->pInspectGrid, i, j))
              Game_inspect(this, i, j);

            // Marks the tile as inspected
            if(pField->aNumbers[i][j] >= 0)
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
 * @param   { int }        x        The tile's x-coordinate in index notation.
 * @param   { int }        y        The tile's y-coordinate in index notation.
*/
void Game_addFlag (Game *this, int x, int y) {
  if(!Grid_getBit(this->gameField.pInspectGrid, x, y))
    Grid_setBit(this->gameField.pFlagGrid, x, y, 1);
}

/**
 * Removes a flag from a tile only if it currently has a flag.
 * Also marks it as uninspected.
 * 
 * @param   { Game * }      this      The game object to be modified.
 * @param   { int }         x         The tile's x-coordinate in index notation.
 * @param   { int }         y         The tile's y-coordinate in index notation.
*/
void Game_removeFlag (Game *this, int x, int y) {
  if(Grid_getBit(this->gameField.pFlagGrid, x, y))
    Grid_setBit(this->gameField.pFlagGrid, x, y, 0);
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

