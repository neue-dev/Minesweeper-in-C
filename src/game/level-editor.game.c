/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-21 7:22:20
 * @ Modified time: 2024-03-25 9:31:06
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
}

/**
 * Checks if the file name of the level to be created is valid.
 * 
 * @param   { Field * }     sName       The file name of the level to be created.
*/
void LevelEditor_isValidName(char *sName) {

}

/**
 * Checks if the name of the level to be created already exists.
 * 
 * @param   { Field * }     sName       The file name of the level to be created.
*/
void LevelEditor_nameExists(char *sName) {
    
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

}

/**
 * Handles errors involved in creating and editing a level.
 * 
*/
void LevelEditor_handleError(ErrorType eErrorType) {

}


