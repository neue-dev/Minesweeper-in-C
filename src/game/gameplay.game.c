/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-21 7:16:46
 * @ Modified time: 2024-03-26 13:45:51
 * @ Description:
 * 
 * Executes tasks involved in-game.
 */

#ifndef GAMEPLAY_
#define GAMEPLAY_

#include "field.obj.h"

#include "../utils/utils.file.h"
#include "../utils/utils.grid.h"

#include <stdio.h>
#include <string.h>

typedef enum GameType GameType;
typedef enum GameDifficulty GameDifficulty;

// Game types chosen by the user
enum GameType {
    GAME_TYPE_CLASSIC,      // Classic game type
    GAME_TYPE_CUSTOM        // Custom game type
};

enum GameDifficulty {
    GAME_DIFFICULTY_EASY,           // Classic game: Easy
    GAME_DIFFICULTY_DIFFICULT,      // Classic game: Difficult
    GAME_DIFFICULTY_NONE            // Custom game: No difficulty
};

/**
 * Sets the game type based on the player's input.
 * 
 * @return	{ GameType }            The game type.
*/
GameType Gameplay_setType() {
    GameType eGameType;

    // TODO: acquire the user's input here
 
    return eGameType;
}

/**
 * Sets the game's difficulty based on the player's input.
 * 
 * @return	{ GameDifficulty }      The game difficulty.
*/
GameDifficulty Gameplay_setDifficulty() {
    GameDifficulty eGameDifficulty;

    // TODO: acquire the user's input here
 
    return eGameDifficulty;
}

/**
 * Sets up the game's field.
 * 
 * @param   { Field * }             pField               A pointer to the game's field.
 * @param   { Game Type }           eGameType            Game type chosen by the player.
 * @param   { Game Difficulty }     eGameDifficulty      Game difficulty chosen by the player.
*/
void Gameplay_setUpField(Field *pField, GameType eGameType, GameDifficulty eGameDifficulty) {

    // Game type: Classic
    if(eGameType == GAME_TYPE_CLASSIC) {

        // Game difficulty: Easy
        if(eGameDifficulty == GAME_DIFFICULTY_EASY) {
            Field_init(pField, 8, 10);      // Sets the field's size to 8 rows x 10 cols
            Field_populate(pField, 10);     // Populates the field with 10 mines
        
        // Game difficulty: Difficult
        } else {
            Field_init(pField, 10, 15);     // Sets the field's size to 10 rows x15 cols
            Field_populate(pField, 35);     // Populates the field with 35 mines
        }

    // Game type: Custom
    } else {

        // Acquires the file of the chosen custom level
        File *pCustomLevel = Gameplay_selectCustomLevel();

        // Sets up the field based on the custom level's specs
        Gameplay_setUpCustomLevel(pCustomLevel, pField);

    }
}

/**
 * Selects the custom level based on the player's input.
 * 
 * @param   { File * }      A pointer to the custom level's file.
 * @return  { File * }      A pointer to the custom level's file.
*/
File *Gameplay_selectCustomLevel() {
    char *sPath;        // The path of the file

    // TODO: acquire the inputted file name

    // Completes the file path with the file name.
    sprintf(sPath, sizeof(*sPath) + sizeof(char)*13, "./levels/%s.txt", sPath);

    File *pCustomLevel = File_create(sPath);

    return pCustomLevel;
}

/**
 * Sets up the custom level's field.
 * 
 * @param   { File * }      pCustomLevel    A pointer to the game's field.
 * @param   { Field * }     pField          A pointer to the game's field.
*/
void Gameplay_setUpCustomLevel(File *pCustomLevel, Field *pField) {
    int i, j;
    char dCell;
    int dWidth, dHeight;

    FILE *pFile = fopen(pCustomLevel->sPath, "r");

    if(pFile == NULL)
        return;     // TODO: error handling

    // Gets the width and height of the field
    fscanf(pFile, "%d %d", &dWidth, &dHeight);

    // Places the mines on the field
    for(i = 0; i < dWidth; i++) {
        for(j = 0; j < dHeight; j++) {

            fscanf(pFile, "%c ", &dCell);

            // Sets the specific bit of the field to 1 if there is a mine
            if(dCell == "X")
                Grid_setBit(pField->pMineGrid, i, j, 1);

            // Sets the specific bit of the field to 0 if there is no mine
            else
                Grid_setBit(pField->pMineGrid, i, j, 0);
        }
    }

}

/**
 * Initiates the game. Handles the player's actions.
 * 
*/
void Gameplay_play() {

}

/**
 * Inspects a cell for a mine.
 * 
*/
void Gameplay_inspect() {
    // TODO: Code this function considering the GUI.
}

/**
 * Adds a flag on a cell.
 * 
*/
void Gameplay_addFlag() {
    // TODO: Code this function considering the GUI.
}

/**
 * Removes a flag from a cell.
 * 
*/
void Gameplay_removeFlag() {
    // TODO: Code this function considering the GUI.
}

/**
 * Quits the game.
 * 
*/
void Gameplay_quit() {
    // TODO: Code this function considering the GUI.
}

/**
 * Ends the game.
 * 
*/
void Gameplay_end() {
    // TODO: Code this function considering the GUI.
}

#endif
