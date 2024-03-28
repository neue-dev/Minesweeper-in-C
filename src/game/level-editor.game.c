/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-21 7:22:20
 * @ Modified time: 2024-03-28 21:06:35
 * @ Description:
 * 
 * Enables the player to create a custom level.
 */

#ifndef LEVEL_EDITOR_
#define LEVEL_EDITOR_

#include "../utils/utils.grid.h"

#include <stdio.h>
#include <string.h>

#define LEVELS_FILE_PATH "src/data/levels.data.txt"

#define LEVELS_FOLDER_PATH "src/data/levels/"
#define LEVELS_FOLDER_PATH_LENGTH   strlen(LEVELS_FOLDER_PATH)

#define LEVEL_NAME_MAX_LENGTH   100
#define LEVEL_NAME_MAX_SIZE     sizeof(char)*(LEVEL_NAME_MAX_LENGTH + 2) // +2 includes \n and \0

// +1 includes \0
#define LEVEL_FILE_PATH_MAX_LENGTH  (LEVEL_NAME_MAX_LENGTH + LEVELS_FOLDER_PATH_LENGTH + 1) 
#define LEVEL_FILE_PATH_MAX_SIZE    sizeof(char)*(LEVEL_FILE_PATH_MAX_LENGTH + 1)

/**
 * Creates a new custom level.
 * 
 * @param   { char * }  sName   Name of the to-be-created level.
*/
void LevelEditor_createLevel(char *sName) {
    int dWidth, dHeight;    // Width and height of a field
    int x, y;               // The x and y coordinates of the tile where a mine is placed
    Grid *pMines;           // Locations where mines are placed
    int bFinished;          // Determines whether the level can now be saved

    // Checks if the name of the to-be-created level already exists
    LevelEditor_levelExists(sName);

    // TODO: input width and height of the field

    // TODO: input locations where mines are to be placed
    while(!bFinished) {

        // Initializes the mine grid
        Grid_clear(pMines, 0);

        // TODO: event-handling
        // TODO: input x and y coordinates
        LevelEditor_placeMine(pMines, x, y);
    }

    // Checks if the level is valid
    LevelEditor_countMines(pMines, dWidth, dHeight);

    // Saves the level according to the inputted data
    LevelEditor_saveLevel(sName, dWidth, dHeight, pMines);
}

/**
 * Checks if a certain level already exists.
 * 
 * @param   { char * }  sKey   Name of the level to search for.
*/
void LevelEditor_levelExists(char *sKey) {

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
            LevelEditor_nameExists();
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
void LevelEditor_saveLevel(char *sName, int dWidth, int dHeight, Grid *pMines) {
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
            fprintf("%c", GridgetBit(pMines, i, j) ? 'X' : '.');

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
 * Places a mine on a tile.
 * 
 * @param   { int }     pMines      Grid where the mines are placed.
 * @param   { int }     x           The x-coordinate of the tile.
 * @param   { int }     y           The y-coordinate of the tile.   
*/
void LevelEditor_placeMine(Grid *pMines, int x, int y) {
    Grid_setBit(pMines, x, y, 1);
}

/**
 * Returns the number of mines on the field.
 * 
 * @param   { Grid * }      pMines      Grid where the mines are placed.
 * @param   { int }         dWidth      Width of the field.
 * @param   { int }         dHeight     Height of the field.
 * @return  { int }         dMines      Number of mines on the grid.
*/
int LevelEditor_countMines(Grid *pMines, int dWidth, int dHeight) {
    int i, j;
    int dMines = 0; // Number of mines on the grid

    // Loops through each tile
    for(i = 0; i < dHeight; i++) {
        for(j = 0; j < dWidth; j++) {

            // Increments dMines when a mine is found
            if(Grid_getBit(pMines, i, j))
                dMines++;
        }
    }

    // Checks if there are no mines at all or if they completely fill up the field
    if(dMines == 0 || dMines == dWidth*dHeight)
        LevelEditor_invalidLevel();
}

/**
 * An error-handling function.
 * Operates when a level already exists and thus cannot be created.
 * 
*/
void LevelEditor_nameExists() {
    // TODO: Code this function considering the inputs and GUI.
    //       Display the error message.
}

/**
 * An error-handling function.
 * Operates when a level is considered invalid.
 * 
*/
void LevelEditor_invalidLevel() {
    // TODO: Code this function considering the inputs and GUI.
    //       Display the error message.
}

#endif