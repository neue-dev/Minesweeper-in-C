/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-28 10:55:29
 * @ Modified time: 2024-03-28 20:52:04
 * @ Description:
 * 
 * Holds the game struct that stores all of the game state.
 */

#ifndef GAME_
#define GAME_

#include "./field.obj.h"
#include "../utils/utils.grid.h"

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
    GAMEPLAY_TYPE_CLASSIC,      // Classic game type
    GAMEPLAY_TYPE_CUSTOM        // Custom game type
};

enum GameDifficulty {
    GAMEPLAY_DIFFICULTY_EASY,           // Classic game: Easy
    GAMEPLAY_DIFFICULTY_DIFFICULT       // Classic game: Difficult
};

enum GameOutcome {
    GAMEPLAY_OUTCOME_QUIT,           // The game ends by the player quitting manually
    GAMEPLAY_OUTCOME_LOSS,           // The game ends by the player losing
    GAMEPLAY_OUTCOME_WIN             // The game ends by the player winning
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
void Game_init(Game *this) {
  this->dTime = 0;

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

