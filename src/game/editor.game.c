/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-21 7:22:20
 * @ Modified time: 2024-03-30 12:50:01
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
#define LEVELS_FILE_PATH "./build/levels/levels.data.txt"
#define LEVELS_FOLDER_PATH "src/data/levels/"
#define LEVELS_FOLDER_PATH_LENGTH   strlen(LEVELS_FOLDER_PATH)

#define LEVEL_NAME_MAX_LENGTH   100
#define LEVEL_NAME_MAX_SIZE     sizeof(char)*(LEVEL_NAME_MAX_LENGTH + 2)    // +2 includes \n and \0

#define LEVEL_FILE_PATH_MAX_LENGTH  (LEVEL_NAME_MAX_LENGTH + LEVELS_FOLDER_PATH_LENGTH)
#define LEVEL_FILE_PATH_MAX_SIZE    sizeof(char)*(LEVEL_FILE_PATH_MAX_LENGTH + 2)

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
 * Registers a new custom level into the levels.data.txt file and saves it's contents in the same directory.
 * If the level exists, the function terminates.
 * 
 * @param   { Game * }  this        The game object to read data from.
 * @param   { char * }  sFilename   Name of the to-be-created level.
 * @return  { int }                 Whether or not the operation was successful.
*/
int Editor_register(Game* this, char *sFilename) {
  int dWidth = this->field.dWidth;
  int dHeight = this->field.dHeight;
  int x, y;

  // Checks if the name of the to-be-created level already exists
  if(Editor_levelExists(sFilename)) {
    this->eError = EDITOR_ERROR_FILENAME_EXISTS;
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
  if(!Editor_saveLevel(this, sFilename, dWidth, dHeight))
    return 0;
  return 1;
}

/**
 * Checks if a certain level already exists.
 * 
 * @param   { Game * }  this        The game object to read data from.
 * @param   { char * }  sLevelKey   Name of the level to search for.
 * @return  { int }                 Whether or not the operation was successful.
*/
int Editor_levelExists(Game *this, char *sLevelKey) {
  int i;

  // Stores the data of the levels file
  File *pLevelsFile;
  int nLevelsCount = 0;
  char *sLevelsArray[LEVELS_MAX_COUNT + 1];

  // Check if file exists first
  if(!File_exists(LEVELS_FILE_PATH)) {
    if(!File_newFile(LEVELS_FILE_PATH)) {
      this->eError;
      return 0;
    }
  }

  // Refers to the file we want
  pLevelsFile = File_create(LEVELS_FILE_PATH);

  // Read the text inside
  File_readText(pLevelsFile, LEVELS_MAX_COUNT + 1, &nLevelsCount, sLevelsArray);

  // Too many levels already
  if(nLevelsCount + 1 > LEVELS_MAX_COUNT) {
    this->eError = EDITOR_ERROR_LEVELS_TOO_MANY;
    return 0;
  }

  printf("Hello world\n");

  // Output all the contents
  for(i = 0; i < nLevelsCount; i++) {
    printf(sLevelsArray[i]);
  }

  return 1;
}

/**
 * Sets the size of a field.
 * 
 * @param   { char * }  sName     Name of the level.
 * @param   { int }     dWidth    The field's width.
 * @param   { int }     dHeight   The field's height.
 * @param   { Grid * }  pMines    Grid where the mines are placed.
*/
int Editor_saveLevel(char *sName, int dWidth, int dHeight, Grid *pMines) {
	// int i, j;

	// // Path of the custom level's file
	// char *sPath = String_alloc(LEVEL_FILE_PATH_MAX_LENGTH);

	// // Completes the path of the level's file
	// snprintf(sPath, LEVEL_FILE_PATH_MAX_SIZE, "%s%s.txt", LEVELS_FOLDER_PATH, sName);

	// // Creates and writes on the level's file
	// FILE *pLevel = fopen(sPath, "r");

	// if(pLevel == NULL)
	// 		return; // TODO: error-handling

	// // Prints the width and height of the field onto the file
	// fprintf("%d %d\n", dHeight, dWidth);

	// // Prints the mines onto the text file
	// for(i = 0; i < dHeight; i++) {
	// 		for(j = 0; j < dWidth; j++) {

	// 				// Prints 'X' for tiles with mines and '.' for tiles without mines
	// 				// fprintf("%c", GridgetBit(pMines, i, j) ? 'X' : '.');

	// 				// Prints space between tiles and a new line every after each row (except the last)
	// 				fprintf("%c", (i == dHeight - 1 && j != dWidth - 1) ? '\n' : ' ');
	// 		}
	// }

	// // Deallocates the memory of the path's string
	// String_kill(sPath);

	// fclose(pLevel);

	// // We will now append the level's name onto the level list
	// FILE *pLevels = fopen(LEVELS_FOLDER_PATH, "a");

	// if(pLevel == NULL)
	// 		return; // TODO: error-handling

	// // Prints the name of the new custom level onto the text file
	// fprintf(pLevels, "%s\n", sName);

	// fclose(pLevels);

  // // !to remove
  // return 0;
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