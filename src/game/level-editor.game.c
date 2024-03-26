/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-21 7:22:20
 * @ Modified time: 2024-03-26 16:31:36
 * @ Description:
 * 
 * Enables the player to create a custom level.
 */

#ifndef LEVEL_EDITOR_
#define LEVEL_EDITOR_

#include "../utils/utils.grid.h"
#include "../utils/utils.string.h"

#include <stdio.h>
#include <string.h>

#define LEVELS_FILE_PATH "src/game/levels/levels.txt"
#define LEVELS_FOLDER_PATH "src/game/levels/"
#define LEVELS_FOLDER_LENGTH 16

#define NAME_MAX_LENGTH 1 << 4
#define NAME_MAX_SIZE sizeof(char)*(NAME_MAX_LENGTH + 1)

#define PATH_MAX_LENGTH LEVELS_FOLDER_LENGTH + NAME_MAX_LENGTH + 4
#define PATH_MAX_SIZE sizeof(char)*(PATH_MAX_LENGTH + 1)

typedef enum ErrorType ErrorType;

enum ErrorType {
    ERROR_TYPE_INVALID_NAME,
    ERROR_TYPE_NAME_EXISTS,
    ERROR_TYPE_INVALID_FIELD
};

struct {
    int dWidth;
    int dHeight;
    int dMines;

    Grid *pMineGrid;
} CustomLevel;

/**
 * Creates a new custom level.
 * 
*/
void LevelEditor_createLevel() {
    char *sFileName;        // Inputted name of the to-be-created custom level

    // TODO: user action - input file name (don't create the file yet!)

    LevelEditor_isValidName(sFileName); // Checks if the name of the level is valid
    LevelEditor_nameExists(sFileName);  // Checks if the name of the level exists

    // TODO: user action - input width and height (5-10, 5-15) (don't save this in the file yet!)

    CustomLevel.pMineGrid = Grid_create(CustomLevel.dWidth, CustomLevel.dHeight);

    // TODO: user action - place and delete mines (don't save this in the file yet!)

    // TODO: user action - save level
    
    LevelEditor_isValidField();         // Checks if the number of mines placed is valid
    LevelEditor_saveFile(sFileName);    // Creates and saves the file for the custom level
}

/**
 * Checks if the file name of the level to be created is valid.
 * 
 * Valid file names solely consist of alphanumeric characters, hyphens, and underscores.
 * 
 * @param   { char * }     sName       The file name of the level to be created.
*/
void LevelEditor_isValidName(char *sName) {

    // Checks if the name's length exceeds the max or is 0
    if(strlen(sName) > NAME_MAX_LENGTH || strlen(sName) == 0)
        LevelEditor_invalidName();

    //Checks if the name contains invalid characters
    int i;
    for(i = 0; i < strlen(sName); i++) {
        
        if(!((sName[i] >= 'A' && sName[i] <= 'Z') ||
             (sName[i] >= 'a' && sName[i] <= 'z') ||
             (sName[i] >= '0' && sName[i] <= '9') ||
              sName[i] == '-' || sName[i] == '_')) {
                LevelEditor_invalidName();
        }
    }
}

/**
 * Checks if the name of the level to be created already exists.
 * 
 * @param   { char * }     sKey       The file name of the level to be created.
*/
void LevelEditor_nameExists(char *sKey) {
    
    // Opens the text file containing the list of level names
    FILE *pLevels = fopen(LEVELS_FILE_PATH, "r");
    
    char sName[NAME_MAX_LENGTH + 1] = {};

    if(pLevels == NULL)
        return; // TODO: error handling

    // Concatinates the key's string with \n, considering that fgets does this.
    // This is to prevent runtime errors with strcmp().
    strcat(sKey, "\n");

    // Checks if sKey matches any of the names in the level list
    while(!feof(pLevels)) {
        fgets(sName, NAME_MAX_SIZE, pLevels);

        // Does error-handling when the name already exists in the level list
        if(strcmp(sKey, sName) == 0) {
            LevelEditor_nameExists(sKey);
            return; // Ends the function once the name has been found
        }
    }

    fclose(pLevels);
}

/**
 * Checks if the number of mines placed in the field is valid.
 * 
*/
void LevelEditor_isValidField() {

    // If the mines fully occupy the grid or do not exist, the level is considered invalid.
    // If the level is invalid, error-handling will occur.
    if(CustomLevel.dMines == CustomLevel.dWidth * CustomLevel.dHeight ||
       CustomLevel.dMines == 0)
        LevelEditor_invalidField();
}

/**
 * Saves the custom level file.
 * 
 * @param   { char * }  sFileName   Name of the custom level to be saved.
*/
void LevelEditor_saveFile(char *sFileName) {

    // Creates a pointer to the path's string
    char *sPath = String_alloc(PATH_MAX_LENGTH);

    // Concatinates the folder path and file name to create the full file path
    snprintf(sPath, PATH_MAX_SIZE, "%s%s.txt", LEVELS_FOLDER_PATH, sFileName);

    // Creates and opens the file for the newly-created custom level
    FILE *pLevel = fopen(sPath, "w");

    if(pLevel == NULL)
        return; // TODO: error-handling

    // Deallocates sPath's memory since it now has no use in this function
    String_kill(sPath);

    // Prints the field's width and height onto the level's text file
    fprintf(pLevel, "%d %d\n", CustomLevel.dWidth, CustomLevel.dHeight);

    // Prints the field's mines onto the level's text file
    int i, j;
    for(i = 0; i < CustomLevel.dHeight; i++) {
        for(j = 0; j < CustomLevel.dWidth; j++) {

            // Prints "X" in mine locations, and "." otherwise
            fprintf(pLevel, Grid_getBit(CustomLevel.pMineGrid, i, j) ? "X" : ".");

            // Prints " " in between the X's and .'s, and "\n" after each row (except the last one)
            fprintf(pLevel, (j == CustomLevel.dWidth - 1 &&
                             i != CustomLevel.dHeight - 1) ? "\n" : " ");
        }
    }

    fclose(pLevel);

    // Opens the text file containing the list of custom levels
    pLevel = fopen(LEVELS_FILE_PATH, "a");

    if(pLevel == NULL)
        return; // TODO: error-handling

    // Adds the level's name to the file
    fprintf(pLevel, "\n%s", sFileName);

    fclose(pLevel);
}

/**
 * An error-handling function. Operates when the name of the to-be-created level is invalid.
 * 
*/
void LevelEditor_invalidName() {
    
}

/**
 * An error-handling function. Operates when the name of the to-be-created level already exists.
 * 
 * @param   { char * }      sName       Supposed name of the new level.
*/
void LevelEditor_nameExists(char *sName) {

}

/**
 * An error-handling function. Operates when the mine placement in the field is invalid.
 * 
*/
void LevelEditor_invalidField() {

}

#endif
