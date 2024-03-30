/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-28 10:55:29
 * @ Modified time: 2024-03-30 00:30:21
 * @ Modified time: 2024-03-31 03:01:32
 * 
 * Holds the game struct that stores all of the game state.
 */

#ifndef GAME_
#define GAME_

#include "./field.obj.h"
#include "./profile.game.c"

#include "../utils/utils.grid.h"
#include "../utils/utils.types.h"
#include "../utils/utils.string.h"

#include <math.h>

#define LEVELS_MAX_COUNT (1 << 10)
#define LEVELS_MAX_NAME_LENGTH (1 << 8)
#define LEVELS_MAX_PATH_LENGTH (1 << 9)
#define LEVELS_FILE_PATH "./build/levels/.levels.data.txt"
#define LEVELS_FOLDER_PATH "./build/levels/"

#define GAME_CELL_HEIGHT 2
#define GAME_CELL_WIDTH 4

#define GAME_EASY_ROWS     8
#define GAME_EASY_COLUMNS  8
#define GAME_EASY_MINES    10
#define GAME_DIFFICULT_ROWS    10
#define GAME_DIFFICULT_COLUMNS 15 
#define GAME_DIFFICULT_MINES   35

#define GAME_MIN_ROWS 5
#define GAME_MAX_ROWS 10
#define GAME_MIN_COLUMNS 5
#define GAME_MAX_COLUMNS 15

typedef enum GameType GameType;
typedef enum GameDifficulty GameDifficulty;
typedef enum GameOutcome GameOutcome;
typedef enum EditorError EditorError;

typedef struct Game Game;

// Game types chosen by the user
enum GameType {
  GAME_TYPE_CLASSIC,            // Classic game type
  GAME_TYPE_CUSTOM,             // Custom game type
  GAME_TYPE_EDITOR,
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

enum EditorError {
  EDITOR_ERROR_NONE,                    // No error
  EDITOR_ERROR_NO_FILE,                 // Could not create file/does not exist
  EDITOR_ERROR_NOT_FOUND,               // The specified file to load does not exist
  EDITOR_ERROR_INVALID_DIMENSIONS,      // Too big or too small grid
  EDITOR_ERROR_FILENAME_INVALID,        // Invalid chars
  EDITOR_ERROR_FILENAME_EXISTS,         // ALready exists
  EDITOR_ERROR_FILENAME_TOO_SHORT,      // Filename too short (0 chars)
  EDITOR_ERROR_FILENAME_TOO_LONG,       // Filename too long
  EDITOR_ERROR_MINES_TOO_MANY,          // Full grid
  EDITOR_ERROR_MINES_NONE,              // 0 mines
  EDITOR_ERROR_LEVELS_TOO_MANY,         // Max levels saved
  EDITOR_ERROR_COULD_NOT_CREATE_FILE,   // Could not create a new file for a level
};

struct Game {
  Profile *pProfile;                            // Holds information about who's currently playing
  Field field;                                  // Game field
  char sSaveName[LEVELS_MAX_NAME_LENGTH + 1];   // Used when editing a grid
  
  int dPauseOffset;                             // How many seconds paused in total
  int dCursorX, dCursorY;                       // The cursor of the player
  int dLastX, dLastY;                           // Indicator for exploded mine
  int dFrameCount, dLastFPS;                    // FPS counter
  int dTimeTaken;
  int bIsSaved;                                 // Has the game been saved (not for editing)
  
  time_t startTime, endTime;                    // Used for computing the time
  time_t pauseStartTime, pauseEndTime;          // Used for accounting for pauses
  time_t frameStart, frameEnd;                  // Used for computing FPS

  EditorError eError;                           // Used for editing the game
  GameType eType;
  GameDifficulty eDifficulty;   
  GameOutcome eOutcome;                         // Some data about the game
};

/**
 * Initializes the game object.
 * 
 * @param   { Game * }  this  The game object.
*/
void Game_setup(Game *this, GameType eGameType, GameDifficulty eGameDifficulty) {

  // Set the game params
  this->eOutcome = GAME_OUTCOME_PENDING;
  this->eType = eGameType;
  this->eDifficulty = eGameDifficulty;

  // The game's cursor
  this->dCursorX = 0;
  this->dCursorY = 0;
  this->dLastX = 0;
  this->dLastY = 0;

  // Start the timer
  time(&this->startTime);
  time(&this->endTime);
  time(&this->frameStart);
  time(&this->frameEnd);

  // Set fps counters
  this->dFrameCount = 0;
  this->dLastFPS = 0;
  this->dPauseOffset = 0;
  this->dTimeTaken = 0;
  this->bIsSaved = 0;

  // CLear the save name first
  String_clear(this->sSaveName);
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
        
      // Sets up the field's width and height and populate with mines
      Field_init(&this->field, GAME_EASY_COLUMNS, GAME_EASY_ROWS);
      Field_populateRandom(&this->field, GAME_EASY_MINES);
    
    // For difficult mode
    } else {

      // Sets up the field's width and height and populate with mines
      Field_init(&this->field, GAME_DIFFICULT_COLUMNS, GAME_DIFFICULT_ROWS);
      Field_populateRandom(&this->field, GAME_DIFFICULT_MINES);
    }
  }

  // Compute the numbers for the field
  Field_setNumbers(&this->field);
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

  // Saves the outcome to the game data
  this->eOutcome = eOutcome;
}

/**
 * Clears the mines and the flags.
 * 
 * @param   { Game * }  this  The game object.
*/
void Game_clear(Game *this) {
  Field_clearFlags(&this->field);
  Field_clearMines(&this->field);
}

/**
 * Checks whether or not the game has been finished. 
 * 
 * @param   { Game * }  this  The game object.
 * @return  { int }           Whether or not the player has finished the game.
*/
int Game_hasWon(Game *this) {
  int x, y;

  // Check if all non-mine cells have been inspected
  // OR if there are flags that aren't mines
  for(x = 0; x < this->field.dWidth; x++) {
    for(y = 0; y < this->field.dHeight; y++) {
      
      // If it hasn't been inspected, and it doesn't have a mine
      if(!Grid_getBit(this->field.pInspectGrid, x, y) &&
        !Grid_getBit(this->field.pMineGrid, x, y)) {
        
        // User hasn't won
        return 0;
      }
    } 
  }

  // The user has already won, so place flags on all mines
  for(x = 0; x < this->field.dWidth; x++) {
    for(y = 0; y < this->field.dHeight; y++) {
      
      // Place a flag if it has a mine
      if(Grid_getBit(this->field.pMineGrid, x, y))
        Grid_setBit(this->field.pFlagGrid, x, y, 1);
    } 
  }

  // All checks were passed
  return 1;
}

/**
 * Checks just the enum.
 * 
 * @param   { Game * }  this  The game object.
 * @return  { int }           Whether or not the game was won.
*/
int Game_isWon(Game *this) {
  return this->eOutcome == GAME_OUTCOME_WIN;
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
  Field *pField = &this->field;

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
        dNumber = this->field.aNumbers[y][x];
        dNumber = dNumber < 0 ? 'X' : dNumber + 48;   // X for mines
        dNumber = dNumber == 48 ? '.' : dNumber;      // Add a dot for 0's

        // If the cell hasn't been inspected, turn it into a space
        if(!Grid_getBit(this->field.pInspectGrid, x, y))
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

  Field *pField = &this->field;

  // If there is a flag there, don't inspect it
  if(Grid_getBit(pField->pFlagGrid, x, y))
    return;

  // Checks if a mine has been inspected
  if(Grid_getBit(pField->pMineGrid, x, y)) {

    // Save the location of the blown up mine
    this->dLastX = x;
    this->dLastY = y;

    // Ends the game
    Game_end(this, GAME_OUTCOME_LOSS);

    return;
  }
  
  // Considers the specific tile inspected
  Field_inspect(pField, x, y);

  // Cascades the inspection if the number on the tile is 0
  if(pField->aNumbers[y][x] == 0) {

    // The following loops check each adjacent tile within bounds of the field
    // Loops through each row
    for(i = x - 1; i <= x + 1; i++) {
      if(i >= 0 && i <= pField->dWidth - 1) {

        // Loops through each column
        for(j = y - 1; j <= y + 1; j++) {
          if(j >= 0 && j <= pField->dHeight - 1) {
            
            // Remove flags obliterated by inspection
            Grid_setBit(this->field.pFlagGrid, i, j, 0);

            // Recurses the function if the number on the tile is 0
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

  // The user has cleared the board
  if(Game_hasWon(this))
    this->eOutcome = GAME_OUTCOME_WIN;
}

/**
 * Adds a flag on a tile only if it hasn't been inspected.
 * 
 * @param   { Game * }     this     The game object to be modified.
*/
void Game_addFlag (Game *this) {
  if(!Grid_getBit(this->field.pInspectGrid, this->dCursorX, this->dCursorY))
    Grid_setBit(this->field.pFlagGrid, this->dCursorX, this->dCursorY, 1);
}

/**
 * Removes a flag from a tile only if it currently has a flag.
 * 
 * @param   { Game * }      this      The game object to be modified.
*/
void Game_removeFlag(Game *this) {
  if(Grid_getBit(this->field.pFlagGrid, this->dCursorX, this->dCursorY))
    Grid_setBit(this->field.pFlagGrid, this->dCursorX, this->dCursorY, 0);
}

/**
 * Pauses the game.
 * 
 * @param   { Game * }  this  The game object to modify.
*/
void Game_pause(Game *this) {
  time(&this->pauseStartTime);
}

/**
 * Unpauses the game.
 * 
 * @param   { Game * }  this  The game object to modify.
*/
void Game_unpause(Game *this) {
  time(&this->pauseEndTime);

  this->dPauseOffset += difftime(this->pauseEndTime, this->pauseStartTime);
}

/**
 * Increments the cursor along the x axis (to the right).
 * Skips tiles that have already been inspected.
 * 
 * @param   { Game * }  this  The game object to modify.
*/
void Game_incrementX(Game *this) {
  // int dCursorXPrev = this->dCursorX;
  int dCursorXNew = this->dCursorX;
  // int dCursorY = this->dCursorY;
  
  // Look for a tile that hasn't been inspected
  // do {
    dCursorXNew = (dCursorXNew + 1) % this->field.dWidth;
  // } while(Grid_getBit(this->field.pInspectGrid, dCursorXNew, dCursorY) && dCursorXNew != dCursorXPrev);

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
  // int dCursorXPrev = this->dCursorX;
  int dCursorXNew = this->dCursorX;
  // int dCursorY = this->dCursorY;
  
  // Look for a tile that hasn't been inspected
  // do {
    dCursorXNew = (dCursorXNew - 1 + this->field.dWidth) % this->field.dWidth;
  // } while(Grid_getBit(this->field.pInspectGrid, dCursorXNew, dCursorY) && dCursorXNew != dCursorXPrev);

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
  // int dCursorYPrev = this->dCursorY;
  int dCursorYNew = this->dCursorY;
  // int dCursorX = this->dCursorX;
  
  // Look for a tile that hasn't been inspected
  // do {
    dCursorYNew = (dCursorYNew + 1) % this->field.dHeight;
  // } while(Grid_getBit(this->field.pInspectGrid, dCursorX, dCursorYNew) && dCursorYNew != dCursorYPrev);

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
  // int dCursorYPrev = this->dCursorY;
  int dCursorYNew = this->dCursorY;
  // int dCursorX = this->dCursorX;
  
  // Look for a tile that hasn't been inspected
  // do {
    dCursorYNew = (dCursorYNew - 1 + this->field.dHeight) % this->field.dHeight;
  // } while(Grid_getBit(this->field.pInspectGrid, dCursorX, dCursorYNew) && dCursorYNew != dCursorYPrev);

  // If found, set the cursor there
  this->dCursorY = dCursorYNew;
}

/**
 * How many characters wide the grid is.
 * 
 * @param   { Game * }   this   The game object to read.
 * @return  { int }             The width of the field in characters.
*/
int Game_getCharWidth(Game *this) {
  return this->field.dWidth * GAME_CELL_WIDTH + 1;
}

/**
 * How many characters tall the grid is.
 * 
 * @param   { Game * }   this   The game object to read.
 * @return  { int }             The height of the field in characters.
*/
int Game_getCharHeight(Game *this) {
  return this->field.dHeight * GAME_CELL_HEIGHT + 1;
}

/**
 * Returns whether or not the game has ended.
 * 
 * @param   { Game * }  this  The game object.
 * @return  { int }           Whether or not the game has ended.
*/
int Game_isDone(Game *this) {
  return this->eOutcome != GAME_OUTCOME_PENDING;
}

/**
 * Returns the time elapsed since the game started.
 * 
 * @param   { Game * }   this   The game object to read.
 * @return  { char * }          A string describing the time elapsed.
*/
char *Game_getTime(Game *this) {
  char *sTimeString = String_alloc(16);

  // Update the timer ONLY if not done
  if(!Game_isDone(this))
    time(&this->endTime);

  // Get the difference between the times
  this->dTimeTaken = round(difftime(this->endTime, this->startTime)) - this->dPauseOffset;

  // Create the time string
  sprintf(sTimeString, (this->dTimeTaken % 60) < 10 ? "%d:0%d" : "%d:%d", 
    this->dTimeTaken / 60, 
    this->dTimeTaken % 60);

  return sTimeString;
}

/**
 * Returns the number of mines left.
 * 
 * @param   { Game * }   this   The game object to read.
 * @return  { char * }          A string describing the mines left.
*/
char *Game_getMinesLeft(Game *this) {
  char *sMineString = String_alloc(16);
  int dMinesLeft = this->field.dMines - Grid_getCount(this->field.pFlagGrid);
  
  // Create the string
  sprintf(sMineString, "%d mines", dMinesLeft);

  return sMineString;
}

/**
 * Returns how many fps the game is running at.
 * Default is 32.
 * 
 * @param   { Game * }   this   The game object to read.
 * @return  { char * }          A string describing the frame rate of the game.
*/
char *Game_getFPS(Game *this) {
  char *sFPSString = String_alloc(16);
  
  // Get the time now
  time(&this->frameEnd);

  // It's been a second
  if(difftime(this->frameEnd, this->frameStart)) {
    this->dLastFPS = this->dFrameCount + 1;
    this->dFrameCount = 0;  

    // Get the time for the next frame  
    time(&this->frameStart);

  // It hasn't been a second. keep counting frames
  } else {
    this->dFrameCount++;
  }

  // Create the string and return it
  sprintf(sFPSString, "%d fps", this->dLastFPS);
  
  return sFPSString;
}

/**
 * Quits the current game.
 * This sets the outcome of the game to GAME_OUTCOME_QUIT.
 * 
 * @param   { Game * }  this  The game object.
*/
void Game_quit(Game *this) {
  this->eOutcome = GAME_OUTCOME_QUIT;
}

/**
 * "Saves" the current game.
 * Sets the save state to 1.
 * 
 * @param   { Game * }  this  The game object.
*/
int Game_save(Game *this) {
  int dLastState = this->bIsSaved;
  this->bIsSaved = 1;

  return dLastState;
}

/**
 * Returns a description of how the game ended.
 * 
 * @param   { Game * }  this  The game object.
 * @return  { char * }        Describes how the game ended.
*/
char *Game_getEndMessage(Game *this) {
  
  // Switch through different endings
  switch(this->eOutcome) {
    case GAME_OUTCOME_LOSS:
      return "You.stepped.on.a.mine!";

    case GAME_OUTCOME_WIN:
      return "You.cleared.all.the.mines!";

    case GAME_OUTCOME_QUIT:
      return "You.quit.the.game!";

    case GAME_OUTCOME_PENDING:
      return "The.game.hasn't.ended...";

    default:
      return "Hmmm,something.went.wrong.";
  } 
}

#endif