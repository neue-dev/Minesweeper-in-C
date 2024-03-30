/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-21 7:22:20
 * @ Modified time: 2024-03-30 13:53:53
 * @ Description:
 * 
 * Enables the player to create a custom level.
 * These are functions the Game class doesn't have but that the editor needs.
 */

#ifndef LEVEL_EDITOR_
#define LEVEL_EDITOR_

#include "./game.c"

#include "../utils/utils.file.h"
#include "../utils/utils.grid.h"

#include <stdio.h>
#include <string.h>

#define LEVELS_MAX_COUNT (1 << 10)
#define LEVELS_MAX_NAME_LENGTH (1 << 8)
#define LEVELS_MAX_PATH_LENGTH (1 << 9)
#define LEVELS_FILE_PATH "./build/levels/levels.data.txt"
#ifndef LEVELS_FOLDER_PATH
#define LEVELS_FOLDER_PATH "./build/levels/"
#endif

/**
 * Initializes the game object.
 * 
 * @param   { Game * }  this  The game object.
*/
void Editor_setup(Game *this, char *sFilename) {

  // Set the game params
  this->eOutcome = GAME_OUTCOME_PENDING;
  this->eType = GAME_TYPE_EDITOR;
  this->eDifficulty = GAME_DIFFICULTY_NONE;
  this->eError = EDITOR_ERROR_NONE;
  strcpy(this->sFilename, sFilename);

  // The game's cursor
  this->dCursorX = 0;
  this->dCursorY = 0;
}

/**
 * Sets the size of the game field.
 * 
 * @param   { Game * }  this      The game object.
 * @param   { int }     dWidth    The width of the field.
 * @param   { int }     dHeight   The height of the field.
*/
void Editor_init(Game *this, int dWidth, int dHeight) {
  int x, y;

  // Init the field
  Field_init(&this->field, dWidth, dHeight);

  // Mark all bits as inspectd
  for(x = 0; x < dWidth; x++)
    for(y = 0; y < dHeight; y++)
      Field_inspect(&this->field, x, y);

  // Compute the numbers
  Field_setNumbers(&this->field);
}

/**
 * Checks if a certain level already exists or if there are too many levels files already.
 * 
 * @param   { Game * }  this        The game object to read data from.
 * @param   { char * }  sLevelName  Name of the level to search for.
 * @return  { int }                 Whether or not the level can be added.
*/
int Editor_canAddLevel(Game *this, char *sLevelName) {
  int i, j;

  // Stores the data of the levels file
  File *pLevelsFile;
  int nLevelsCount = 0;
  char sLevelEntry[LEVELS_MAX_NAME_LENGTH + 1];
  char *sLevelsArray[LEVELS_MAX_COUNT + 1];

  // Check if file exists first
  if(!File_exists(LEVELS_FILE_PATH)) {
    if(!File_newFile(LEVELS_FILE_PATH)) {
      this->eError = EDITOR_ERROR_NO_FILE;
      return 0;
    }
  }

  // Refers to the file we want
  pLevelsFile = File_create(LEVELS_FILE_PATH);

  // Read the text inside
  File_readText(pLevelsFile, LEVELS_MAX_COUNT + 1, &nLevelsCount, sLevelsArray);

  // Too many levels already
  if(nLevelsCount + 1 > LEVELS_MAX_COUNT) {
    File_kill(pLevelsFile);
    this->eError = EDITOR_ERROR_LEVELS_TOO_MANY;
    return 0;
  }

  // Check all the contents
  for(i = 0; i < nLevelsCount; i++) {
    String_clear(sLevelEntry);

    // Copy the name
    j = 0;
    while(sLevelsArray[i][j] != ';')
      sprintf(sLevelEntry, "%s%c", sLevelEntry, sLevelsArray[i][j++]);

    // The same name
    if(!strcmp(sLevelEntry, sLevelName)) {
      File_kill(pLevelsFile);
      this->eError = EDITOR_ERROR_FILENAME_EXISTS;
      return 0;
    }
  }

  // Clean up
  File_kill(pLevelsFile);

  // Filename does not yet exist
  return 1;
}

/**
 * Returns the number of mines on the field.
 * Returns 0 if no mines or if there are too many mines.
 * Max number of mines is dWidth * dHeight - 1.
 * 
 * @param   { Grid * }      pMines      Grid where the mines are placed.
 * @return  { int }         dMines      Number of mines on the grid.
*/
int Editor_countMines(Game *this) {
	int x, y;
	int dMines = 0;		// Number of mines on the grid

	// Loops through each tile
	for(x = 0; x < this->field.dWidth; x++) {
		for(y = 0; y < this->field.dHeight; y++) {

			// Increments dMines when a mine is found
			if(Grid_getBit(this->field.pMineGrid, x, y))
				dMines++;
		}
	}
  
	return dMines;
}

/**
 * Loads a level unto the game object.
*/

/**
 * Saves a level into the levels folder.
 * 
 * @param   { Game * }  this        Where to read the mine data from.
 * @param   { char * }  sLevelName  Name of the level.
 * @return  { int }                 Whether or not the operation was successful.
*/
int Editor_saveLevel(Game *this, char *sLevelName) {
	int i, j;
  char sPath[LEVELS_MAX_PATH_LENGTH] = { 0 };
  File *pLevelFile;
  
  // Some stuff about the buffer we're going to write
  int nRows = this->field.dHeight;          // How many rows we actually have
  int nColumns = this->field.dWidth;        // How many columns we have
  char *sLevelArray[GAME_MAX_ROWS + 1];     // The buffer that stores what we want to write

	// Completes the path of the level's file
	snprintf(sPath, LEVELS_MAX_PATH_LENGTH, "%s%s.txt", LEVELS_FOLDER_PATH, sLevelName);

	// If the file could not be created
	if(!File_newFile(sPath)) {
    this->eError = EDITOR_ERROR_COULD_NOT_CREATE_FILE;
    return 0;
  }

  // Grab the level file
  pLevelFile = File_create(sPath);

  // Clear the file
  File_clear(pLevelFile);

  // Append the width and height first
  sLevelArray[0] = String_alloc(nColumns + 1);
  sprintf(sLevelArray[0], "%d %d\n", nRows, nColumns);

  // Create the data
  for(i = 1; i <= nRows; i++) {
    sLevelArray[i] = String_alloc(nColumns + 1);
    
    // Add the 'X' and '.'
    for(j = 0; j < nColumns; j++)
      sprintf(sLevelArray[i], "%s%c", sLevelArray[i], 
        Grid_getBit(this->field.pMineGrid, j, i) ? 'X' : '.');

    // New line
    sprintf(sLevelArray[i], "%s%c", sLevelArray[i], '\n');
  }

	// Write the data
  File_writeText(pLevelFile, nRows, sLevelArray);

  // Garbage collection
  for(i = 0; i <= nRows; i++)
    String_kill(sLevelArray[i]);
  File_kill(pLevelFile);

  return 1;
}

/**
 * Registers a new custom level into the levels.data.txt file and saves it's contents in the same directory.
 * If the level exists, the function terminates.
 * 
 * @param   { Game * }  this        The game object to read data from.
 * @param   { char * }  sLevelName  Name of the to-be-created level.
 * @return  { int }                 Whether or not the operation was successful.
*/
int Editor_register(Game* this, char *sLevelName) {
  int x, y;
  int dWidth = this->field.dWidth;
  int dHeight = this->field.dHeight;

  // The new entry to write
  char *sLevelEntry[1];
  File *pLevelsFile;

  // Checks if the name of the to-be-created level already exists
  if(!Editor_canAddLevel(this, sLevelName)) {
    
    // The error is already set by the above function call
    return 0;
  }

  // Checks if the level is valid
  if(!Editor_countMines(this)) {
    this->eError = EDITOR_ERROR_MINES_NONE;
    return 0;

  // Board is full of mines
  } else if(Editor_countMines(this) == dWidth * dHeight) {
    this->eError = EDITOR_ERROR_MINES_TOO_MANY;
    return 0;
  }

  // Saves the level according to the inputted data
  // This function already sets the error too
  if(!Editor_saveLevel(this, sLevelName)) {
    return 0;
  }

  // We know it should exist by now so this should be fine
  pLevelsFile = File_create(LEVELS_FILE_PATH);

  // Edit the original levels file to reflect registration
  sLevelEntry[0] = String_alloc(LEVELS_MAX_NAME_LENGTH + 2);
  sprintf(sLevelEntry[0], "%s;\n", sLevelName);
  File_writeText(pLevelsFile, 1, sLevelEntry);

  // Clean up
  File_kill(pLevelsFile);

  // Success
  return 1;
}

/**
 * Adds a mine on a tile and recomputes numbers.
 * 
 * @param   { Game * }     this     The game object to be modified.
*/
void Editor_addMine(Game *this) {
  if(!Grid_getBit(this->field.pMineGrid, this->dCursorX, this->dCursorY))
    Grid_setBit(this->field.pMineGrid, this->dCursorX, this->dCursorY, 1);

  // Recompute numbers
  Field_setNumbers(&this->field);
}

/**
 * Removes a mine from a tile and recomputes numbers.
 * 
 * @param   { Game * }      this      The game object to be modified.
*/
void Editor_removeMine(Game *this) {
  if(Grid_getBit(this->field.pMineGrid, this->dCursorX, this->dCursorY))
    Grid_setBit(this->field.pMineGrid, this->dCursorX, this->dCursorY, 0);

  // Recompute numbers
  Field_setNumbers(&this->field);
}

#endif