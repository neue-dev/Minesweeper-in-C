/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-21 7:22:20
 * @ Modified time: 2024-03-24 17:22:20
 * @ Description:
 * 
 * Enables the player to create a custom level.
 */

#include "../utils/utils.file.h"
#include "../utils/utils.string.h"

#include <stdio.h>
#include <string.h>

typedef enum ErrorType ErrorType;

enum ErrorType {
    ERROR_TYPE_INVALID_NAME,
    ERROR_TYPE_NAME_EXISTS,
    ERROR_TYPE_INVALID_FIELD
};

void LevelEditor_createLevel() {
    char *sFileName;

    // TODO: input file name

    LevelEditor_isValidName(sFileName);         // Checks if the name of the level is valid
    LevelEditor_nameExists(sFileName);          // Checks if the name of the level exists

    // TODO: enter number of rows and columns

    // TODO: place mines
    // TODO: delete mines

    // TODO: check if the mine placement is valid
    // TODO: finish and save

}

/**
 * Checks if the file name of the level to be created is valid.
 * 
 * @param   { Field * }     sName       The file name of the level to be created.
*/
void LevelEditor_isValidName(char *sName) {
    int i;

    // Checks if all the characters of sName are capital letters.
    while(sName[i] != '\0') {
        if(!(sName[i++] >= 'A' && sName[i++] <= 'B'))
            LevelEditor_handleError(ERROR_TYPE_INVALID_NAME);
    }
}

/**
 * Checks if the name of the level to be created already exists.
 * 
 * @param   { Field * }     sName       The file name of the level to be created.
*/
void LevelEditor_nameExists(char *sName) {
    int i = 0;
    FILE *pLevels = fopen("./levels/levels.txt", "r");
    char dummy[1] = "";

    fscanf(pLevels, "%s", dummy);
    while(!feof(pLevels)) {
        //
    }

    fclose(pLevels);
}

/**
 * Checks if the number of mines placed in the field is valid.
 * 
*/
void LevelEditor_isValidField() {

}

/**
 * Saves the custom level file.
 * 
*/
void LevelEditor_saveFile() {
    // TODO: add the file name to level.txt
}

/**
 * Handles errors involved in creating and editing a level.
 * 
*/
void LevelEditor_handleError(ErrorType eErrorType) {

}


