/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-21 7:22:20
 * @ Modified time: 2024-03-30 12:11:35
 * @ Description:
 * 
 * Enables the player to create a custom level.
 * These are functions the Game class doesn't have but that the editor needs.
 */

#ifndef LEVEL_EDITOR_
#define LEVEL_EDITOR_

#include "./game.c"

#include "../utils/utils.grid.h"

#include <stdio.h>
#include <string.h>

#define LEVELS_FILE_PATH "src/data/levels.data.txt"

#define LEVELS_FOLDER_PATH "src/data/levels/"
#define LEVELS_FOLDER_PATH_LENGTH   strlen(LEVELS_FOLDER_PATH)

#define LEVEL_NAME_MAX_LENGTH   100
#define LEVEL_NAME_MAX_SIZE     sizeof(char)*(LEVEL_NAME_MAX_LENGTH + 2)    // +2 includes \n and \0

#define LEVEL_FILE_PATH_MAX_LENGTH  (LEVEL_NAME_MAX_LENGTH + LEVELS_FOLDER_PATH_LENGTH)
#define LEVEL_FILE_PATH_MAX_SIZE    sizeof(char)*(LEVEL_FILE_PATH_MAX_LENGTH + 2)

typedef enum EditorError EditorError;

enum EditorError {
  EDITOR_ERROR_LEVEL_EXISTS,
  EDITOR_ERROR_COULD_NOT_CREATE_FILE,
};

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
 * Creates a new custom level.
 * 
 * @param   { char * }  sName   Name of the to-be-created level.
*/
void Editor_createLevel(char *sName) {
    int dWidth, dHeight;    // Width and height of a field
    int x, y;               // The x and y coordinates of the tile where a mine is placed
    Grid *pMines;           // Locations where mines are placed
    int bFinished;          // Determines whether the level can now be saved

    // Checks if the name of the to-be-created level already exists
    Editor_levelExists(sName);

    // TODO: input width and height of the field

    // TODO: input locations where mines are to be placed
    while(!bFinished) {

        // Initializes the mine grid
        Grid_clear(pMines, 0);

        // TODO: event-handling
        // TODO: input x and y coordinates
        // Editor_placeMine(pMines, x, y);
    }

    // Checks if the level is valid
    Editor_countMines(pMines, dWidth, dHeight);

    // Saves the level according to the inputted data
    Editor_saveLevel(sName, dWidth, dHeight, pMines);
}

/**
 * Checks if a certain level already exists.
 * 
 * @param   { char * }  sKey   Name of the level to search for.
*/
void Editor_levelExists(char *sKey) {

    // Opens the text file containing the list of levels
    FILE *pLevels = fopen(LEVELS_FILE_PATH, "r");

    if(pLevels == NULL)
        return; // TODO: error-handling
    
    // Concatinates the key's string with \n, considering that fgets does this.
    // This is to prevent runtime errors with strcmp().
    strcat(sKey, "\n");

    // Name of the level to be checked
    char *sName = String_alloc(LEVEL_NAME_MAX_LENGTH);

    while(!feof(pLevels)) {
        fgets(sName, LEVEL_NAME_MAX_SIZE, pLevels);

        // Checks if the name already exists in the list
        if(strcmp(sName, sKey) == 0) {
            // Editor_nameExists();
            return; // Exits the function
        }

    }

    // Deallocates the memory of the name's string
    String_kill(sName);

    fclose(pLevels);


}

/**
 * Sets the size of a field.
 * 
 * @param   { char * }  sName     Name of the level.
 * @param   { int }     dWidth    The field's width.
 * @param   { int }     dHeight   The field's height.
 * @param   { Grid * }  pMines    Grid where the mines are placed.
*/
void Editor_saveLevel(char *sName, int dWidth, int dHeight, Grid *pMines) {
	int i, j;

	// Path of the custom level's file
	char *sPath = String_alloc(LEVEL_FILE_PATH_MAX_LENGTH);

	// Completes the path of the level's file
	snprintf(sPath, LEVEL_FILE_PATH_MAX_SIZE, "%s%s.txt", LEVELS_FOLDER_PATH, sName);

	// Creates and writes on the level's file
	FILE *pLevel = fopen(sPath, "r");

	if(pLevel == NULL)
			return; // TODO: error-handling

	// Prints the width and height of the field onto the file
	fprintf("%d %d\n", dHeight, dWidth);

	// Prints the mines onto the text file
	for(i = 0; i < dHeight; i++) {
			for(j = 0; j < dWidth; j++) {

					// Prints 'X' for tiles with mines and '.' for tiles without mines
					// fprintf("%c", GridgetBit(pMines, i, j) ? 'X' : '.');

					// Prints space between tiles and a new line every after each row (except the last)
					fprintf("%c", (i == dHeight - 1 && j != dWidth - 1) ? '\n' : ' ');
			}
	}

	// Deallocates the memory of the path's string
	String_kill(sPath);

	fclose(pLevel);

	// We will now append the level's name onto the level list
	FILE *pLevels = fopen(LEVELS_FOLDER_PATH, "a");

	if(pLevel == NULL)
			return; // TODO: error-handling

	// Prints the name of the new custom level onto the text file
	fprintf(pLevels, "%s\n", sName);

	fclose(pLevels);
}

/**
 * Returns the number of mines on the field.
 * Returns 0 if no mines or if there are too many mines.
 * Max number of mines is dWidth * dHeight - 1.
 * 
 * @param   { Grid * }      pMines      Grid where the mines are placed.
 * @param   { int }         dWidth      Width of the field.
 * @param   { int }         dHeight     Height of the field.
 * @return  { int }         dMines      Number of mines on the grid.
*/
int Editor_countMines(Game *this, int dWidth, int dHeight) {
	int i, j;
	int dMines = 0;		// Number of mines on the grid

	// Loops through each tile
	for(i = 0; i < dHeight; i++) {
		for(j = 0; j < dWidth; j++) {

			// Increments dMines when a mine is found
			if(Grid_getBit(this->field.pMineGrid, i, j))
				dMines++;
		}
	}

	// Checks if there are no mines at all or if they completely fill up the field
	if(!dMines || dMines == dWidth * dHeight)
		return 0;
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